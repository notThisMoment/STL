# STL概论
## 模板的作用
模板可以用来特化,指定下一个参数的类型.很多有关类外的类的类型的事情都可以使用模板.

## 临时对象的产生和应用
**制造临时对象**
在型别对象后面直接加一对小括号,并指定初值.STL最常将此技巧应用于仿函数与算法的搭配上.
特别是仿函数,经常被用于临时对象的产生和使用.

## 静态常量整数成员直接在函数内部初始化

## 前闭后开区间表示法[)
任何一个STL算法,都需要获得由一对迭代器所标示的区间,用于表示操作范围.这一对迭代器所标示的是个所谓的前闭后开区间.

# 空间配置器
空间配置器不仅在分配内容,甚至可以直接读和取磁盘和其他辅助介质.

## 具备次配置力的SGI空间配置器
SGI STL的每一个容器都已经指定其缺省的空间配置器未alloc.比如下面的vector声明:
```C++
	template <class T, class Alloc = alloc>		// 缺省使用alloc为配置器
	class vector {};
```
### SGI 特殊的空间配置器, std::alloc
为了精密分工,STL allocator将两个阶段操作区分开来.
内存配置操作由alloc::allocate()负责,内存释放操作由alloc::deallocate()负责.
对象构造操作由::construct()负责,对象析构操作由::destroy()负责.

### 构造和析构基本工具: construct() 和 destroy()
> The  destructor for class T is trivial if all of the following is true:
> 1. The destructor is not user-provided.
> 2. The destructor is not virtual 
> 3. All direct base classes have trivial destructors.
> 4. All non-static date members of class type (or array of class type) have trivial destructors
> A trivialk destructor is a destructor that performs no action. Objects with trivial destryctors don`t require a delete-expression and mat be disposed of by simply deallocating their storage . All data types compatible with the C language are trivially destructible.

上述construct()接受一个指针 p 和一个初值 value, 该函数被用于将初值设定到指针指定的空间上.
destroy() 会首先利用 value_type()获得迭代器所指对象的类型,再使用__type_traits<T>判断该类别的析构函数是否trivial.若是(__true_type),就什么都不做直接结束.

### 空间的配置和释放

对象构造前的空间配置和对象析构后的空间释放的设计考虑如下:

* 向system heap要求空间
* 考虑多线程的状态
* 考虑内存空间不足时的措施
* 考虑过多"小型区块"可能造成的内存碎片问题

针对内存破碎,SGI设计了双层级配置器.  

第一级配置器直接使用malloc()和free()

第二级配置器因地制宜:当配置区块>128bytes,视之为"足够大",就调用第一级配置器.当配置区块<128bytes,视之为"过小".采用复杂的memory pooling的整理方式.

整个设计是只开放第一级配置器,还是同时开放第二级配置器,取决于 __USE_MALLOC是否被定义:

```c++
# ifdef __USE_MALLOC
...
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;												// 令 alloc 为第一级配置器

# else
typedef __default_alloc_template<__NODE_ALLOCATOR_THREAD, 0> alloc;		    // 第二级配置器

# endif  /*__USE_MALLOC*/
/*
	alloc 不接受任何template类型参数	
*/
```

无论alloc被配置为第一级配置器还是第二级配置器,SGI还会为它再包装一个接口如下,使配置器的接口能够符合STL的规格:

```c++
template<class T, class Alloc>
class simple_alloc 
{
    public:
    static T* allocate(size_t n)
    { return 0 == n ? 0 : (T*) Alloc::allocate(n * sizeof(T) ); }
    static T* allocate(void)
    { return (T*) Alloc::allocate(sizeof (T) ); }
    static void deallocate(T* p, size_t n)
    { 
        if (0 != n) 
    		Alloc::deallocate(p, n * sizeof(T) );
    }
    static void deallocate(T* p)
    { Alloc:deallocate(p, sizeof (T) ); }
}
```

### 第一级配置器 __malloc_alloc_template剖析

第一级配置器以malloc(),free(),realloc()等C函数执行实际的内存配置,释放,重配置操作,并实现出类似C++ new-handler 的机制.

> 第一级配置器不能直接运用 C++ new-handler 机制，因为它不用::operator new来配置内存。
>
> C++ new-handler机制是：
>
> 我可以要求系统在内存配置需求无法被满足的时候，调用一个我指定的函数。

>​	SGI第一级配置器的allocate()和realloc()都是在调用malloc()和realloc()不成功后，改调用oom_realloc()和oom_realloc()，这两个都有内循环，不断调用“内存不足处理例程”(在这个例程里面会一直尝试释放内存)，期望在某次调用后，能够获得足够的内存。
>
>​	但是如果没有“内存不足处理例程”，oom_realloc()和oom_realloc()会直接丢出bad_alloc异常信息，或者使用exit(1)强行终止程序.
>
>​	所以设计和和设定"内存不足处理例程"是客端的责任

### 第二级配置器 __default_alloc_template 剖析

SGI第二级配置器的做法是次层配置:对于<128bytes的区块,以内存池管理.每次配置一大块内存,并维护对应的自由链表.链表的每个节点如下:

```c++
union obj
{
    union obj* free)list_link;
    char client_data[1]; 			
}
```

在本例中,维护的区块大小为free-list={8,16,..,128},也就是说,一共会有16个free-list;

### 空间配置函数allocate()

allocate()是__default_alloc_template的标准接口.

```c++
static void* allcoate(size_t n)						// n > 0
{
    obj* volatile* my_free_list;					// volatile 保证队特殊地址的稳定访问
    obj* result;
    
    // 大于128, 调用第一级配置器
    if ( n > (size_t) __MAX_BYTES)
    {
        return(malloc_alloc::allocate(n));
    }
    // 寻找16个free list 中最适合的一个
    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if (result = 0)
    {
        // 没找到可用的free list,准备重新填充free list
        void* r = refill(ROUND_UP(n));
        return r;
    }
    
    // 调整 free list
    *my_free_list= result -> free_list_link;
    return (result);
}
```

### 空间释放函数

没啥好说的

### 重新填充fill lists

![内存池配置](%E5%86%85%E5%AD%98%E6%B1%A0%5B%E9%85%8D%E7%BD%AE.jpg)

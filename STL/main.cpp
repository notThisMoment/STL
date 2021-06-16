#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

template <class T>
struct MyIter
{
    typedef T value_type;
    T* ptr;
    MyIter(T* p=0) : ptr(p) {}
    T& operator* () const { return *ptr; }
};

template <class I>
typename I::value_type func( I iter)
{
    return *iter;
}

/*
// 偏特化
template <class T>
struct iterator_traits<T*>
{
    typedef T value_type;
};

template <class I>
typename iterator_traits<I>::value_type
func(I ite)
{ return *ite; }

*/

// 五个作为标记使用的型别（tag types)
/*
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
*/

template <class ForwardIterator, class Distance>
inline void __advance(ForwardIterator& i, Distance n,
                      input_iterator_tag)
{
    while (n--) ++i;
}

template <class ForwardIterator, class Distance>
inline void __advance(ForwardIterator& i, Distance n,
                      forward_iterator_tag)
{
    advance(i, n, input_iterator_tag());
}

template <class BidiectionIterator, class Distance>
inline void __advance(BidiectionIterator& i, Distance n,
                      bidirectional_iterator_tag)
{
    // 双向， 逐一前进
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}

template <class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n,
                      random_access_iterator_tag)
{
    // 双向， 跳跃前进
    i += n;
}

/*
 * 注意上述语法， 每个__advance() 的最后一个参数都只声明型别，并未指定参数名称，
 * 因为它纯粹用来激活重载机制，函数中根本不会使用到该参数。
 * */

// 下面是一个对外开放的上层控制接口，调用上述各个重载的__advance()
template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n)
{
    __advance(i, n,
              iterator_traits<InputIterator>::iterator_category());
}

// STL list 的 node 结构
template <class T>
struct __list_node
{
    typedef void* void_pointer;
    void_pointer prev;
    void_pointer next;
    T data;
};

// list迭代器的设计
template<class T, class Ref, class Ptr>
struct __list_iterator
{
        typedef __list_iterator<T, T&, T*>      iterator;
        typedef __list_iterator<T, Ref, Ptr>    self;

        typedef bidirectional_iterator_tag  iterator_category;
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef __list_node<T*> link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        link_type node;             // 迭代器内部指向 list 节点的普通指针

        __list_iterator(link_type x) : node(x) {}
        __list_iterator() {}
        __list_iterator(const iterator& x) : node(x.node) {}

        bool operator==(const self& x) const { return node == x.node; }
        bool operator!=(const self& x) const { return node != x.node; }
        // 以下对迭代器取值（dereference)， 取节点的数据值
        reference operator*() const { return (*node).data; }

        // 以下是迭代器的成员存取(member access)运算子的标准做法。
        pointer operator->() const { return &(operator*()); }

};

void sort(list<int>& list1)
{
    int fill = 0;
    list<int> carry;
    list<int> counter[64];

    while (!list1.empty())
    {
        carry.splice(carry.begin(), list1, list1.begin());
        int i = 0;
        while (i < fill && !counter[i].empty())
        {
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }
        carry.swap(counter[i]);
        if ( i == fill) ++fill;
    }

    for ( int i = 1; i < fill; ++i)
        counter[i].merge(counter[i-1]);
    carry.swap(counter[fill - 1]);
}

int main() {
    // ...
    /*
     * list<int> list1{2,4,6,1,3,8,4,6,5};
        sort(list1);
        cout << "success" << endl;
     * */
    // test heap
    int ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
    vector<int> ivec(ia, ia + 9);

    make_heap(ivec.begin(), ivec.end());
    for (int i = 0; i < ivec.size(); ++i)
        cout << ivec[i] << ' ';
    cout << endl;

    ivec.push_back(7);
    push_heap(ivec.begin(), ivec.end());
    for (int i = 0; i < ivec.size(); ++i)
        cout << ivec[i] << ' ';
    cout << endl;

    pop_heap(ivec.begin(), ivec.end());
    cout << ivec.back() << endl;
    ivec.pop_back();

    for (int i = 0; i < ivec.size(); ++i)
        cout << ivec[i] << ' ';
    cout << endl;

    sort_heap(ivec.begin(), ivec.end());
    for (int i = 0; i < ivec.size(); ++i)
        cout << ivec[i] << ' ';
    cout << endl;
}

//
// Created by lai on 2021/5/29.
//

#ifndef STL_STL_UNINITIALZED_H
#define STL_STL_UNINITIALZED_H
#include <memory>

class stl_uninitialzed {

};


// 这个函数的进行逻辑：萃取迭代器first的value type，判断该类型是否为POD类型。（POD， 标量类型或者传统的C结构类型）
template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
    return __uninitialized_fill_n(first, n, x , value_type(first));
}


template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

// 如果是 copy construction 等同于 assignment， 而且
// destructor 是 trivial, 以下就有效
// 如果是 POD 类型， 执行流程就会转进到以下函数（POD， 标量类型或者传统的C结构类型）
// 这是由 function template 的参数推导机制而得(?)
template <class ForwardIterator, class Size, class T>
inline ForwardIterator ininitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
    __uninitializaed_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
    {
        return fill_n(first, n, x);
    }
}

// 如果不是 POD 类型， 执行流程就会转进到以下函数
// 这是由 function template 的参数推导机制而得(?)
template <class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
{
    ForwardIterator cur = first;
    for ( ; n > 0; --n, ++cur)
        construct(&*cur, x);
    return cur;
}

// uninitialized_copy()针对 char* 和 wchar_t* 的特化版本
inline char* uninitialized_copy(const char* first, const char* last, char* result)
{
    memove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t * first, const wchar_t * last, wchar_t * result)
{
    memove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);     // ? 没有 sizeof
}


#endif //STL_STL_UNINITIALZED_H

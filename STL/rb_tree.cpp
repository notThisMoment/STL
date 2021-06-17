//
// Created by lai on 2021/6/16.
//

#include "rb_tree.h"
#include "Alloc.h"
#include <cstddef>

typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;

struct __rb_tree_node_base
{
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x)         // 返回这个树里面最小的值。
    {
        while (x->left != 0)    x = x -> left;
        return x;
    }

    static base_ptr maximum(base_ptr x)
    {
        while (x -> right != 0) x = x->right;
        return x;
    }

};

template <class Value>
struct __rb_tree_node : public __rb_tree_node_base
{
    typedef __rb_tree_node<Value>* link_type;
    Value value_field;      // 节点值
};


// 下面是一些迭代器的设计
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};

struct __rb_tree_base_iterator
{
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    base_ptr node;          // 用来与容器之间产生一个连结关系

    void increment()
    {
        if (node->right != 0)           // 如果有右子节点。状况（1）
        {
            node = node->right;         // 就向右走
            while (node->left != 0)     // 然后一直往左子树走到底
                node = node->left;      // 就是结果
        }
        else                                // 没有右子节点。状况（2）
        {
            base_ptr y = node->parent;      // 找出父节点
            while (node == y->right)        // 如果现行节点本身是个右节点
            {

                node = y;                   // 如果现行节点本身是个右子节点
                y = y->parent;              // 就一直上溯，直到“不为右子节点”为止
            }
            if (node->right != y)           // 若此时的右子节点不等于此时的父节点
                node = y;                   // 状况（3），此时的父节点就是解答
                /*
                 * 这里的 “若此时的右子节点不等于此时的父节点” 为了应付一种特殊情况。
                 * 别深究，后面会补充
                 * */
        }
    }

    void decrement()
    {
        if (node->color == __rb_tree_red &&         // 如果是红节点，且
            node->parent->parent == node)           // 父节点的父节点等于自己（？）
            node = node -> right;                   // 状况（1）右子节点就是解答（？）
        // 以上情况发生于 node 为 header时（也就是node 为 end（）时）（？）
        else if (node->left != 0)                   // 如果有左子节点。状况（2）
        {
            base_ptr y = node->left;                // 令 y 指向左子节点
            while (y->right != 0)                   // 当 y 有右子节点时
                y = y->right;                       // 一直往右子节点走到底
            node = y;                               // 最后就是答案
        }
        else                                        // 既不是根节点， 也没有左子节点
        {
            base_ptr y = node->parent;              // 状况（3）找出父节点
            while (node == y->left)                 // 当现行节点身为左子节点
            {
                node = y;                           // 一直交替往上走，直到现行节点不是左子节点。
                y = y->parent;
            }
            node = y;                               // 此时的父节点就是答案
        }
    }
};

// RB-tree 的正规迭代器
template <class Value, class Ref, class Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator
{
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef __rb_tree_iterator<Value, Value&, Value*>   iterator;
    typedef __rb_tree_iterator<Value, const Value&, const Value*>   const_iterator;
    typedef __rb_tree_iterator<Value, Ref, Ptr> self;
    typedef __rb_tree_node<Value>*  link_type;


    __rb_tree_iterator() {}
    __rb_tree_iterator(link_type x) { node = x; }
    __rb_tree_iterator(const iterator& it)   { node = it.node; }

    reference operator*() const { return link_type(node)->value_field; }

# ifndef __SGI_STL_NO_ARROW_FPERATOR
    pointer operator->() const { return &(operator*()); }
# endif

    self& operator++()
    {
        increment();
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        increment();
        return tmp;
    }

    self& operator--()
    {
        decrement();
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
};

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree{
protected:
    typedef void* void_pointer;
    typedef __rb_tree_node_base* base_ptr;
    typedef __rb_tree_node<Value> rb_tree_node;
    typedef simple_alloc<rb_tree_node, Alloc>   rb_tree_node_allocator;
    typedef __rb_tree_color_type color;
public:
    typedef Key key_value;
    typedef Value value_type;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type & const_reference;
    typedef rb_tree_node * link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
protected:
    link_type get_node() { return rb_tree_node_allocator::allocate(); }
    void put_node(link_type p) { return rb_tree_node_allocator::deallocate(p); }

    link_type create_node(const value_type& x)
    {
        link_type tmp = get_node();         // 配置空间
    }

    link_type clone_node(link_type x)       // 复制一个节点（的值和色）
    {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = 0;
        tmp->right = 0;
        return tmp;
    }


};
//
// Created by lai on 2021/6/16.
//

#include "rb_tree.h"
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
struct __rb_tree_node : public _-__rb_tree_node_base
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
        if (node->right != 0)       // 如果有右子节点。状况（1）
        {
            node = node->right;     // 就向右走
            while (node->left != 0) // 然后一直往左子树走到底
                node = node->left;  // 就是结果
        }
        else                        // 没有右子节点。状况（2）
        {
            base_ptr y = node->parent;
            while (node == y->right)
            {
                node = y;
                y = y->parent;
            }
        }
    }
};
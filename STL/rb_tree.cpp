//
// Created by lai on 2021/6/16.
//

#include "rb_tree.h"

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

};

struct __rb_tree_base_iterator
{
};
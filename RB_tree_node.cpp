#include "RB_tree_node.h"
template<class K, class V>
RB_tree_node<K, V>::RB_tree_node() 
{
    key = 0;
    value = 0;
    // parent = nullptr;
    // left = nullptr;
    // right = nullptr;
    color = red;//初始颜色为红色
}

template<class K, class V>
RB_tree_node<K, V>::RB_tree_node(K in_key, V in_value) 
{
    key = in_key;
    value = in_value;
    // parent = nullptr;
    // left = nullptr;
    // right = nullptr;
    color = red;//初始颜色为红色
}
template<class K, class V>
RB_tree_node<K, V>::~RB_tree_node() {}

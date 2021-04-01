#ifndef RB_TREE_NODE_H
#define RB_TREE_NODE_H
#include <memory>
typedef bool color_type;
static constexpr color_type red = true;//红色为true
static constexpr color_type black = false;
template<class K, class V> 
class RB_tree_node
{
public:
    RB_tree_node();
    RB_tree_node(K in_key, V in_value);
    ~RB_tree_node();
    //RB_tree_node<K, V>* parent;//指向父节点的指针
    std::shared_ptr<RB_tree_node<K, V>> parent;
    //weak_ptr<RB_tree_node<K, V>> parent;
    //RB_tree_node<K, V>* left; //指向左子节点的指针
    std::shared_ptr<RB_tree_node<K, V>> left;
    std::shared_ptr<RB_tree_node<K, V>> right;//指向右子节点的指针
    K key;              //键值
    V value;            //节点值
    color_type color;   //节点颜色
};
#endif
#ifndef RB_TREE_CPP
#define RB_TREE_CPP

#include "RB_tree.h"
#include <memory>
// #include "myIterator.h"
// #include "myIterator.cpp"
#include<iostream>
using namespace std;
template<class K, class V>
RB_tree<K, V>::RB_tree()
{
    rb_size = 0;
    //构造函数默认根节点为空
    //root = nullptr;
    //初始化header节点
    header = make_shared<RB_tree_node<K, V>>();
	//header->parent = nullptr;
	header->left = header;
	header->right = header;
    // header->parent(nullptr);
    // header->left(header);
    // header->left(header);
    header->color = red;
    header->key = 0;
    header->value = 0;
}
template<class K, class V>
RB_tree<K, V>::~RB_tree() {
    
    // delete header;
    // delete root;
    // header = nullptr;
    // root = nullptr;
}

template<class K, class V>
int RB_tree<K, V>::getSize() {
    return rb_size;
}


/******************************************************************************************/
/*函数功能：检查是否为红黑树                                                               */
/*入口参数：RB_tree_node<K, V>*& cur_node, black_count, k                                 */
/*返回值：bool                                                                            */
/*****************************************************************************************/
template<class K, class V>
bool RB_tree<K, V>::check(std::shared_ptr<RB_tree_node<K, V>> cur_node, const int black_count, int k) {
    if(cur_node == nullptr) {
        return true;
    }
    //RB_tree_node<K, V>* cur_parent = cur_node->parent;
    std::shared_ptr<RB_tree_node<K, V>> cur_parent = cur_node->parent;
    if(cur_parent != nullptr && cur_parent->color == red && cur_node->color == red) {//违反性质：不能出现连续红节点
        return false;
    }
    if(cur_node->color == black) {
        k++;
    }
    if(cur_node->left == nullptr && cur_node->right == nullptr) {
        if(k != black_count) {
            return false;
        }
    }
    return check(cur_node->left, black_count, k) && check(cur_node->right, black_count, k);//递归判断
}

/******************************************************************************************/
/*函数功能：检查是否为红黑树                                                               */
/*入口参数：RB_tree_node<K, V>*& cur_node                                                 */
/*返回值：bool                                                                            */
/*****************************************************************************************/
template<class K, class V>
bool RB_tree<K, V>::check() {
    if(root == nullptr) {
        return true;
    }
    if(root->color == red) {//违反性质：根节点必须为红色
        return false;
    }
    int black_count = 0;//记录一条路径上黑色节点的数目
    //RB_tree_node<K, V>* cur_node = root;
    std::shared_ptr<RB_tree_node<K, V>> cur_node = root;
    while(cur_node != nullptr) {
        if(cur_node->color == black) {
            black_count++;
        }
        cur_node = cur_node->left;
    }
    return check(root, black_count, 0);

}


/******************************************************************************************/
/*函数功能：根据key中序遍历节点并输出                                                       */
/*入口参数：RB_tree_node<K, V>*& cur_node                                                 */
/*返回值：void                                                                            */
/*****************************************************************************************/
template<class K, class V>
void RB_tree<K, V>::my_inOrder(std::shared_ptr<RB_tree_node<K, V>>& cur_node) {
    if(cur_node != nullptr) {
        my_inOrder(cur_node->left);
        cout<<cur_node->key<<" "<<cur_node->value<<" ";
        if(cur_node->color == true) cout<<"RED"<<endl;
        else cout<<"BLACK"<<endl;
        my_inOrder(cur_node->right);
    }
}

template<class K, class V>
void RB_tree<K, V>::inOrder() {
    my_inOrder(root);
}

/*****************************************************************************************/
/*函数功能：根据key查找对应的节点                                                          */
/*入口参数：key                                                                           */
/*返回值：查找到的节点的指针（不存在则为nullptr）                                           */
/*****************************************************************************************/
template<class K, class V>
std::shared_ptr<RB_tree_node<K, V>> RB_tree<K, V>::Find(const K& key)
{   
    //RB_tree_node<K, V>* cur_node = root;
    std::shared_ptr<RB_tree_node<K, V>> cur_node = root;
    while(cur_node != nullptr) {
        if(key < cur_node->key) {//key对应节点只能在cur_node的左子树中
            cur_node = cur_node->left;
        }
        else if(key > cur_node->key) {
            cur_node = cur_node->right;//key对应节点只能在cur_node的右子树中
        }
        else break;//找到对应的节点
    }
    return cur_node;//若未找到，则返回的是nullptr；否则为对应节点的指针
}


/*****************************************************************************************/
/*函数功能：左旋当前节点x                                                                  */
/*入口参数：RB_tree_node<K ,V>*& cur_node                                                 */
/*形式：
                    /*---------------------------------------*\                
                    |       p                         p       |                
                    |      / \                       / \      |
                    |     x   d    rotate left      y   d     |
                    |    / \       ===========>    / \        |
                    |   a   y                     x   c       |
                    |      / \                   / \          |
                    |     b   c                 a   b         |
                    \*---------------------------------------*/
/*返回值：void                                                                            */
/*****************************************************************************************/
template<class K, class V>
void RB_tree<K, V>::left_rotate(shared_ptr<RB_tree_node<K, V>>& cur_node) {
    //RB_tree_node<K, V>* x = cur_node;//x为当前节点
    shared_ptr<RB_tree_node<K, V>> x = cur_node;
    shared_ptr<RB_tree_node<K, V>> y = x->right;//y为x的右子节点
    //1.修改x和y->left的指向
    if(y != nullptr) {
        x->right = y->left;
        if(y->left != nullptr) {
            y->left->parent = x;//注意：由于是三叉节点，需要同时双向修改；但同时考虑如果为null则不能修改
        }
    }
    //2.接下来修改x的父节点指向为y，或将y设为根节点
    y->parent = x->parent;
    if(x == root) {
        root = y;
    }
    else {
        if(x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
    }
    //3.修改x，y互相的反方向
    y->left = x;
    x->parent = y;
}

/*****************************************************************************************/
/*函数功能：右旋当前节点x                                                                 */
/*入口参数：RB_tree_node<K ,V>*& cur_node                                                */
/*形式：
                    /*----------------------------------------*\
                    |     p                         p          |
                    |    / \                       / \         |
                    |   d   x      rotate right   d   y        |
                    |      / \     ===========>      / \       |
                    |     y   a                     b   x      |
                    |    / \                           / \     |
                    |   b   c                         c   a    |
                    \*----------------------------------------*/
/*返回值：void                                                                            */
/*****************************************************************************************/
template<class K, class V>
void RB_tree<K, V>::right_rotate(shared_ptr<RB_tree_node<K, V>>& cur_node) {
    //RB_tree_node<K, V>* x = cur_node;
    shared_ptr<RB_tree_node<K, V>> x = cur_node;
    //RB_tree_node<K, V>* y = x->left;
    shared_ptr<RB_tree_node<K, V>> y = x->left;
    if(y != nullptr) {
        x->left = y->right;
        if(y->right != nullptr) {
            y->right->parent = x;
        }
    }
    //修改x的父节点和y的指向或将y设为根节点
    y->parent = x->parent;
    if(x == root) {
        root = y;
    }
    else {
        if(x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
    }
    //反转x，y互相指向
    y->right = x;
    x->parent = y;
}


/******************************************************************************************/
/*函数功能：根据key插入节点                                                                */
/*入口参数：key, value                                                                    */
/*返回值：bool类型变量判断是否成功插入成功/修改值                                           */
/*****************************************************************************************/
template<class K, class V>
bool RB_tree<K, V>::insert(const K& key, const V& value) {
    if(root == nullptr) {
        //root = new RB_tree_node<K, V>(key, value);//新建根节点
        root = make_shared<RB_tree_node<K, V>>(key, value);
        root->key = key;
        root->value = value;
        root->color = black;//根节点为黑色
        header->parent = root;
        root->parent = header;
        header->left = root;
        header->right = root;
        rb_size++;
        return true;
    }
    //RB_tree_node<K, V>* cur_parent = nullptr;//标记待找到的插入节点位置的父节点
    std::shared_ptr<RB_tree_node<K, V>> cur_parent;
    //RB_tree_node<K, V>* cur_node = root;
    std::shared_ptr<RB_tree_node<K, V>> cur_node = root;
    while(cur_node != nullptr) {
        if(key < cur_node->key) {
            cur_parent = cur_node;
            cur_node = cur_node->left;
        }
        else if(key > cur_node->key) {
            cur_parent = cur_node;
            cur_node = cur_node->right;
        }
        else {//待插入节点的key值对应的位置已存在节点，则直接进行修改
            cur_node->value = value;
            return true;//修改成功
        }
    }
    //RB_tree_node<K, V>* insert_node = new RB_tree_node<K, V>(key, value);
    //RB_tree_node<K, V>* insert_node = new RB_tree_node<K, V>(key, value);
    std::shared_ptr<RB_tree_node<K, V>> insert_node = make_shared<RB_tree_node<K, V>>(key, value);
    insert_node->key = key;
    insert_node->value = value;
    if(key < cur_parent->key) {
        cur_parent->left = insert_node;
    }
    else {
        cur_parent->right = insert_node;
    }
    insert_node->parent = cur_parent;
    if(cur_parent->color == red) {//如果插入节点的父节点为红色，则需要修复，否则直接结束即可
        insert_fixed(insert_node);
    }
    rb_size++;
    //插入可能会导致header节点相关信息的改变
    header->parent = root;
    root->parent = header;
    header->left = find_smallest_node();
    header->right = find_largest_node();
    return true;
}

/*****************************************************************************************/
/*函数功能：插入节点后使其重新满足红黑树性质                                                */
/*方式：左旋、右旋、变色                                                                   */
/*规则：
    case 1:当前节点为根节点，直接置为黑色并返回
    case 2:当前节点的父节点为黑色，则无条件结束即可
    case 3:当前节点的父节点为红色(言外之意：一定有祖父节点,且为黑)
        case 3.1:父为祖父的左子节点
            case 3.1.1 父左叔右，且父红叔红
            case 3.1.2 父左叔右，且父红叔黑/不存在
                case 3.1.2.1 当前节点为父节点的右节点（左旋后转换为case3.1.2.2处理）
                case 3.1.2.2 当前节点为父节点的左节点
        case 3.2:父为祖父的右子节点（与case 3.1只是方向不同）
            case 3.2.1 父右叔左，且父红叔红（和case3.1.1完全相同）
            case 3.2.2 父右叔左，且父红叔黑/不存在
                case 3.2.2.1 当前节点为父节点的左节点（右旋后转换为case3.2.2.2处理）
                case 3.2.2.2 当前节点为父节点的右节点

/*入口参数：RB_tree_node<K ,V>*& cur_node                                                */
/*返回值：void                                                                           */
/*****************************************************************************************/
template<class K, class V>
//void RB_tree<K, V>::insert_fixed(RB_tree_node<K, V>*& cur_node) {
void RB_tree<K, V>::insert_fixed(std::shared_ptr<RB_tree_node<K, V>>& cur_node) {
    // RB_tree_node<K, V>* cur_parent = nullptr;
    std::shared_ptr<RB_tree_node<K, V>> cur_parent;
    // RB_tree_node<K, V>* cur_uncle = nullptr;
    std::shared_ptr<RB_tree_node<K, V>> cur_uncle;
    // RB_tree_node<K, V>* cur_grandpa = nullptr;
    std::shared_ptr<RB_tree_node<K, V>> cur_grandpa;
    while(cur_node != nullptr) {
        //case 1:当前节点为根节点，直接置为黑色并返回
        if(cur_node == root) {
            cur_node->color = black;
            break;
        }
        //case 2:当前节点的父节点为黑色，则无条件结束即可
        if(cur_node->parent->color == black) {
            break;
        }
        //case 3:当前节点的父节点为红色(言外之意：一定有祖父节点,且为黑)
        cur_parent = cur_node->parent;
        cur_grandpa = cur_parent->parent;
        //case 3.1:父为祖父的左子节点
        if(cur_parent == cur_grandpa->left) {
            cur_uncle = cur_grandpa->right;
            //case 3.1.1 父左叔右，且父红叔红
            if(cur_uncle && cur_uncle->color == red) {
                cur_parent->color = black;
                cur_uncle->color = black;
                cur_grandpa->color = red;
                cur_node = cur_grandpa;
            }
            //case 3.1.2 父左叔右，且父红叔黑/不存在
            else {
                //case 3.1.2.1 当前节点为父节点的右节点（左旋后转换为case3.1.2.2处理）
                if(cur_node == cur_parent->right) {
                    cur_node = cur_parent;//将当前节点置为父节点
                    left_rotate(cur_node);//注意左旋之后原来的cur_parent指针和cur_grandpa已经失效了
                    cur_parent = cur_node->parent;
                    cur_grandpa = cur_parent->parent;
                }
                //case 3.1.2.2 当前节点为父节点的左节点
                //交换父节点和祖父节点颜色
                cur_parent->color = black;
                cur_grandpa->color = red;
                //以祖父节点作为当前节点进行右旋
                cur_node = cur_grandpa;
                right_rotate(cur_node);
                break;
            }
        }
        //case 3.2:父为祖父的右子节点
        else {
            cur_uncle = cur_grandpa->left;
            //case 3.2.1 父右叔左，且父红叔红（和case3.1.1完全相同）
            if(cur_uncle && cur_uncle->color == red) {
                cur_parent->color = black;
                cur_uncle->color = black;
                cur_grandpa->color = red;
                cur_node = cur_grandpa;
            }
            //case 3.2.2 父右叔左，且父红叔黑/不存在
            else {
                //case 3.2.2.1 当前节点为父节点的左节点（右旋后转换为case3.2.2.2处理）
                if(cur_node == cur_parent->left) {
                    cur_node = cur_parent;//将当前节点置为父节点
                    right_rotate(cur_node);//注意右旋之后原来的cur_parent指针和cur_grandpa已经失效了
                    cur_parent = cur_node->parent;
                    cur_grandpa = cur_parent->parent;
                }
                //case 3.2.2.2 当前节点为父节点的右节点
                //交换父节点和祖父节点颜色
                cur_parent->color = black;
                cur_grandpa->color = red;
                //以祖父节点作为当前节点进行左旋
                cur_node = cur_grandpa;
                left_rotate(cur_node);
                break;
            }
        }
    }
}

/*****************************************************************************************/
/*函数功能：根据key删除节点                                                                */
/*入口参数：key                                                                           */
/*删除时寻找的替代节点：待删节点的右子树中最小的节点                                        */
/*返回值：bool类型变量判断是否成功删除                                                     */
/*****************************************************************************************/
template<class K, class V>
bool RB_tree<K, V>::erase(const K& key) {
    //确定待删节点selete_node
    //RB_tree_node<K, V>* delete_node = find(key);
    std::shared_ptr<RB_tree_node<K, V>> delete_node = Find(key);
    if(delete_node == nullptr) return false;//如果没找到待删节点，则直接返回false即可
    rb_size--;//节点数减一
    //RB_tree_node<K, V>* successor_node = nullptr;
    std::shared_ptr<RB_tree_node<K, V>> successor_node;
    //RB_tree_node<K, V>* replace_node = nullptr;
    std::shared_ptr<RB_tree_node<K, V>> replace_node;
    //确定后继节点successor_node（可能为待删节点）
    if(delete_node->left == nullptr || delete_node->right == nullptr) {
        successor_node = delete_node;
    }
    else {
        successor_node = find_successor_node(delete_node);
    }
    //确定替代后继节点的节点replace_node(可能为nullptr)
    if(successor_node->left != nullptr) {
        replace_node = successor_node->left;
    }
    else replace_node = successor_node->right;
    //修改successor_node的父节点和replace_node，或将replace_node置为根节点
    if(replace_node != nullptr) {
        replace_node->parent = successor_node->parent;
    }
    if(successor_node == root) {
        root = replace_node;
        //delete successor_node;//释放内存
        if(root == nullptr) {
            return true;
        }
    }
    else {
        if(successor_node == successor_node->parent->left) {
            successor_node->parent->left = replace_node;
        }
        else {
            successor_node->parent->right = replace_node;
        }
    }
    //用successor_node的key-value填充delete_node的key-value
    if(delete_node != successor_node) {
        delete_node->key = successor_node->key;
        delete_node->value = successor_node->value;
    }
    //根据successor_node的颜色判断是否需要进行修复
    if(successor_node->color == black) {
        erase_fixed(replace_node, successor_node->parent);
    }
    //delete successor_node;
    //更新header节点相关信息
    if(root != nullptr) {
        header->parent = root;
        root->parent = header;
        header->left = find_smallest_node();
        header->right = find_largest_node();
    }

    return true;
}


/*****************************************************************************************/
/*函数功能：删除节点后使其重新满足红黑树性质                                                */
/*方式：左旋、右旋、变色                                                                   */
/*规则：
    case 1 cur_node为红色或为根节点，则直接赋黑色
    case 2 cur_node为黑色/不存在
        case 2.1 兄弟节点为红色 
            case 2.1.1 兄弟为红，且兄弟在右：先交换父亲和兄弟颜色，再左旋父节点，继续循环
            case 2.1.2 兄弟为红，且兄弟在左：先交换父亲和兄弟颜色，再右旋父节点，继续循环
        case 2.2 兄弟节点为黑色
            case 2.2.1兄弟为黑，且兄弟节点的左右节点均不存在/为黑：兄弟为红，转向父节点
            case 2.2.2 兄弟为黑，且兄弟节点在左，兄弟节点的子节点为右红左黑/不存在：交换兄弟和兄弟的右节点颜色，左旋兄弟节点，执行下一步
            case 2.2.3 兄弟为黑，且兄弟节点在左，兄弟节点的子节点为左红右任意：兄弟的左子节点为黑，父节点也为黑，右旋父节点

            case 2.2.4 兄弟为黑，且兄弟节点在右，兄弟节点的子节点为左红右黑/不存在
            case 2.2.5 兄弟为黑，且兄弟节点在右，兄弟节点的子节点为右红左任意
            （2.2.4和2.2.5与2.2.2和2.2.3正好相反）

/*入口参数：RB_tree_node<K ,V>*& cur_node, RB_tree_node<K, V>*& cur_parent                */
/*返回值：void                                                                           */
/*****************************************************************************************/
template<class K, class V>
//void RB_tree<K, V>::erase_fixed(RB_tree_node<K, V>*& cur_node, RB_tree_node<K, V>*& cur_parent) {
void RB_tree<K, V>::erase_fixed(std::shared_ptr<RB_tree_node<K, V>>& cur_node, std::shared_ptr<RB_tree_node<K, V>>& cur_parent) {
    std::shared_ptr<RB_tree_node<K, V>> cur_brother;
    //需要注意由于后继节点为黑色，那么说明cur_brother节点一定存在！！！
    while(cur_node != root) {//当前节点为根节点时直接退出即可
        //case 1 cur_node为红色，或者为根节点，则直接赋黑色
        if(cur_node != nullptr && cur_node->color == red) {
            cur_node->color = black;
            break;
        }
        //case 2 cur_node为黑色/不存在
        //先找兄弟节点
        if(cur_node == cur_parent->left) {
            cur_brother = cur_parent->right;
        }
        else {
            cur_brother = cur_parent->left;
        }
        //case 2.1 兄弟节点为红色 
        if(cur_brother != nullptr && cur_brother->color == red) {
            //RB_tree_node<K, V>* temp = nullptr;
            std::shared_ptr<RB_tree_node<K, V>> temp;
            //case 2.1.1 兄弟为红，且兄弟在右
            if(cur_brother == cur_parent->right) {
                temp = cur_brother->left;
                cur_brother->color = black;
                cur_parent->color = red;
                //cur_node = cur_parent;//注意此时cur_node指向不改变
                left_rotate(cur_parent);
            }
            //case 2.1.2 兄弟为红，且兄弟在左
            else {
                temp = cur_brother->right;
                cur_brother->color = black;
                cur_parent->color = red;//这部分处理和case2.1.1相同
                right_rotate(cur_parent);                 
            }
            cur_brother = temp;//注意：必须更新新的兄弟节点，并且可以推断出必然为黑 
        }
        //case 2.2 兄弟节点为黑色
        //case 2.2.1兄弟为黑，且兄弟节点的左右节点均不存在/为黑
        if( (cur_brother->left == nullptr || cur_brother->left->color == black) &&
            (cur_brother->right == nullptr || cur_brother->right->color == black)  ) {
            cur_brother->color = red;
            cur_node = cur_parent;
            cur_parent = cur_node->parent;//注意！！！迭代时必须同时更新父节点
        }
        else {
            //case 2.2.2 兄弟为黑，且兄弟节点在左，兄弟节点的子节点为右红左黑/不存在
            if(cur_brother == cur_parent->left) {
                if( (cur_brother->right != nullptr && cur_brother->right->color == red) && 
                    (cur_brother->left == nullptr || cur_brother->left->color == black) ) {
                    cur_brother->color = red;
                    cur_brother->right->color = black;
                    left_rotate(cur_brother);
                    cur_brother = cur_parent->left;//注意更新cur_brother
                }
                //case 2.2.3 兄弟为黑，且兄弟节点在左，兄弟节点的子节点为左红右任意
                cur_brother->color = cur_parent->color;//注意此时并不确定cur_parent的颜色
                cur_parent->color = black;
                cur_brother->left->color = black;
                right_rotate(cur_parent);
                break;
            }
            //case 2.2.4 兄弟为黑，且兄弟节点在右，兄弟节点的子节点为左红右黑/不存在
            else {
                if( (cur_brother->left != nullptr && cur_brother->left->color == red) && 
                    (cur_brother->right == nullptr || cur_brother->right->color == black) ) {
                    cur_brother->color = red;
                    cur_brother->left->color = black;
                    right_rotate(cur_brother);
                    cur_brother = cur_parent->right;//注意更新cur_brother
                }
                //case 2.2.5 兄弟为黑，且兄弟节点在右，兄弟节点的子节点为右红左任意
                cur_brother->color = cur_parent->color;//注意此时并不确定cur_parent的颜色
                cur_parent->color = black;
                cur_brother->right->color = black;
                left_rotate(cur_parent);
                break;
            }
        }
    }
    if(cur_node == root) {
        cur_node->color = black;
    }
}

/*****************************************************************************************/
/* 函数功能：找寻当前节点的后继节点                                                        */
/* 入口参数：当前节点                                                                     */
/* 返回值：当前节点的后继节点                                                              */  
/*****************************************************************************************/
template<class K, class V>
shared_ptr<RB_tree_node<K, V>> RB_tree<K, V>::find_successor_node(shared_ptr<RB_tree_node<K, V>>& cur_node) {
	//RB_tree_node<K, V>* temp_node = cur_node->right;
	std::shared_ptr<RB_tree_node<K, V>> temp_node = cur_node->right;
    while(temp_node->left != nullptr)
	{
		temp_node = temp_node->left;
	}
	return temp_node;	
}

/*****************************************************************************************/
/* 函数功能：找寻红黑树中值最小的节点                                                      */
/* 入口参数：无                                                                           */
/* 返回值：指向最小节点的指针                                                              */  
/*****************************************************************************************/
template<class K, class V>
shared_ptr<RB_tree_node<K, V>> RB_tree<K, V>::find_smallest_node()
{
    if(root == nullptr) return nullptr;//如果为空树则直接返回
	//RB_tree_node<K, V>* cur_node = root;
    shared_ptr<RB_tree_node<K, V>> cur_node = root;
    while(cur_node->left != nullptr) {//往左寻找
        cur_node = cur_node->left;
    }
    return cur_node;
}

/*****************************************************************************************/
/* 函数功能：找寻红黑树中值最大的节点                                                      */
/* 入口参数：无                                                                           */
/* 返回值：指向最大节点的指针                                                              */  
/*****************************************************************************************/
template<class K, class V>
shared_ptr<RB_tree_node<K, V>> RB_tree<K, V>::find_largest_node()
{
    if(root == nullptr) return nullptr;//如果为空树则直接返回
	//RB_tree_node<K, V>* cur_node = root;
    shared_ptr<RB_tree_node<K, V>> cur_node = root;
    while(cur_node->right != nullptr) {//往右寻找
        cur_node = cur_node->right;
    }
    return cur_node;
}

//#include "myIterator.h"
// #include "RB_tree.h"
template<class K, class V, class R, class P>
void myIterator<K, V, R, P>::increment() {
   //如果存在右子节点则直接在右子树中搜索
   if(node->right != nullptr) {
        node = node->right;
        while(node->left != nullptr) {
            node = node->left;
        }
    }
    //如果不存在右子树则向上迭代到位于左孩子的父节点
    else {
        //Node* node_parent = node->parent;
        std::shared_ptr<Node> node_parent = node->parent;
        while(node == node_parent->right) {//会一直找到header节点为止，最差的结束条件为node指向header
            node = node_parent;
            node_parent = node->parent;
        }
        if(node->right != node_parent) {//十分巧妙的一句话，解决了根节点和header节点的转换。来源：stl源码
            node = node_parent;
        }
    }
}
template<class K, class V, class R, class P>
void myIterator<K, V, R, P>::decrement() {
    //header结点的情况：自减之后到key最大的结点
    if (node->color == red && node->parent->parent == node) {
        node = node->right;
    }
    //找左子树的最右边的节点
    else if (node->left != nullptr) {
        node = node->left;
        while (node->right != nullptr) {
            node = node->right;
        }
    }
    //如果不存在左子树则向上迭代到位于右孩子的父节点
    else {
        //Node* node_parent = node->parent;
        std::shared_ptr<Node> node_parent = node->parent;
        while (node_parent->left == node) {
            node = node_parent;
            node_parent = node_parent->parent;
        }
        node = node_parent;
    }
}

//前缀++
template<class K, class V, class R, class P>
myIterator<K, V, R, P>& myIterator<K, V, R, P>::operator++() {
    increment();
    return *this;
}

//后缀++
template<class K, class V, class R, class P>
myIterator<K, V, R, P> myIterator<K, V, R, P>::operator++(int) {
    Self temp = *this;
    increment();
    return temp;
}

//前缀--
template<class K, class V, class R, class P>
myIterator<K, V, R, P>& myIterator<K, V, R, P>::operator--() {
    decrement();
    return *this;
}

//前缀--
template<class K, class V, class R, class P>
myIterator<K, V, R, P> myIterator<K, V, R, P>::operator--(int) {
    Self temp = *this;
    decrement();
    return temp;
}

//解引用*
template<class K, class V, class R, class P>
R myIterator<K, V, R, P>::operator*() {
    return node->key;
}

//->
template<class K, class V, class R, class P>
P myIterator<K, V, R, P>::operator->() {
    return &(operator*());
}

//==
template<class K, class V, class R, class P>
bool myIterator<K, V, R, P>::operator==(const myIterator<K, V, R, P>& another_iterator) {
    return node == another_iterator.node;
}

//!=
template<class K, class V, class R, class P>
bool myIterator<K, V, R, P>::operator!=(const myIterator<K, V, R, P>& another_iterator) {
    return node != another_iterator.node;
}


#endif
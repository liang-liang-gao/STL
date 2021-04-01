#ifndef RB_TREE_H
#define RB_TREE_H
#include "RB_tree_node.h"
#include "RB_tree_node.cpp"
#include <memory>
// #include "myIterator.h"

template<class K, class V, class R, class P>
class myIterator
{
    typedef RB_tree_node<K, V> Node;
    typedef myIterator<K, V, R, P> Self;
private:
    std::shared_ptr<Node> node;      //迭代器中包含的指针
    void increment();//寻找有序遍历时下一个节点
    void decrement();//寻找有序遍历时上一个节点
public:
    myIterator(): node(nullptr) {}
    myIterator(std::shared_ptr<Node> in_node): node(in_node) {}
    myIterator(const Self& another_iterator): node(another_iterator.node) {}//必须是const！！！
    //~myIterator() { delete node; }
    //myIterator(Self&& another_iterator) {node = another_iterator.node; another_iterator.node = nullptr;}
    Self& operator++();                           //重载迭代器前缀++运算符
    Self operator++(int);                         //重载后缀++运算符
    Self& operator--();                           //重载迭代器前缀--运算符
    Self operator--(int);                         //重载后缀++运算符
    R operator*();                                //重载解引用运算符,返回的是key值的引用
    P operator->();                               //重载->运算符，返回的是指针类型
    bool operator==(const Self& another_iterator);//重载==运算符
    bool operator!=(const Self& another_iterator);//重载!=运算符
    
};



template<class K, class V> 
class RB_tree
{
private:
    int rb_size;                                   //红黑树节点个数
    std::shared_ptr<RB_tree_node<K, V>> root;                      //红黑树的根节点
    std::shared_ptr<RB_tree_node<K, V>> header;                    //！！！补充内容：header是为了给begin()和end()函数提供方便
    //void my_inOrder(RB_tree_node<K, V>*& cur_node);//中序（有序）输出红黑树节点值
    void my_inOrder(std::shared_ptr<RB_tree_node<K, V>>& cur_node);
    bool check(std::shared_ptr<RB_tree_node<K, V>> cur_node, const int black_count, int k);
public:
    typedef myIterator<K, V, K&, K*> iterator;
    
    RB_tree();                    //构造函数中默认根节点为空,并且需要创造header节点
    ~RB_tree();
    iterator begin() {
        // iterator it(header->left);//不能用下面那种方式，会报错。原因不知道！！！ ->已经知道:上面构造函数参数没有加const的原因
        // return it; 
        return iterator(header->left);//未优化时的详细过程：先创建临时对象t1，得到t1(header->left)，再创建临时对象t2=t1(拷贝构造函数，必须是const-reference)，
    }
    iterator end() {
        // iterator it(header);
        // return it;
        return iterator(header);
    }
    iterator find(const K& key) {//迭代器查找，返回的是迭代器
        //iterator it;
        //RB_tree_node<K, V>* temp = Find(key);
        std::shared_ptr<RB_tree_node<K, V>> temp = Find(key);
        if(temp == nullptr) {
            return end();
        }
        return iterator(temp);
    }
    int getSize();                                    //得到红黑树的节点个数
    bool check();                                     //检查是否满足红黑树性质
    //void left_rotate(RB_tree_node<K, V>*& cur_node);  //左旋
    void left_rotate(std::shared_ptr<RB_tree_node<K, V>>& cur_node);
    //void right_rotate(RB_tree_node<K, V>*& cur_node); //右旋
    void right_rotate(std::shared_ptr<RB_tree_node<K, V>>& cur_node);
    //RB_tree_node<K, V>* Find(const K& key);           //查找函数,返回指针类型
    std::shared_ptr<RB_tree_node<K, V>> Find(const K& key); 
    bool insert(const K& key, const V& value);        //根据key和value插入节点，成功返回true；若已存在则修改节点值为value
    //void insert_fixed(RB_tree_node<K, V>*& cur_node); //修复插入后的结构变化
    void insert_fixed(std::shared_ptr<RB_tree_node<K, V>>& cur_node); 
    bool erase(const K& key);                         //根据key删除节点，若存在节点并删除成功则返回true，否则返回false
    //void erase_fixed(RB_tree_node<K, V>*& cur_node, RB_tree_node<K, V>*& cur_parent);
    void erase_fixed(std::shared_ptr<RB_tree_node<K, V>>& cur_node, std::shared_ptr<RB_tree_node<K, V>>& cur_parent);
    //RB_tree_node<K, V>* find_successor_node(std::shared_ptr<RB_tree_node<K, V>>& cur_node); //找寻当前节点的后继节点
    std::shared_ptr<RB_tree_node<K, V>> find_successor_node(std::shared_ptr<RB_tree_node<K, V>>& cur_node);
    //RB_tree_node<K, V>* find_smallest_node(); //红黑树中值最小的节点
    std::shared_ptr<RB_tree_node<K, V>> find_smallest_node();
    //RB_tree_node<K, V>* find_largest_node();  //红黑树中值最大的节点
    std::shared_ptr<RB_tree_node<K, V>> find_largest_node();
    void inOrder();                           //中序遍历

};


#endif
#ifndef MYMAP_H
#define MYMAP_H
#include "RB_tree.h"
#include "RB_tree.cpp"
template<class Key, class Value>
class myMap
{
private:
    RB_tree<Key, Value> rbt;//map内部的红黑树
public:
    typedef typename RB_tree<Key, Value>::iterator iterator;//告诉编译器iterator是一种类型
    iterator begin() {
        return rbt.begin();
    }
    iterator end() {
        return rbt.end();
    }
    iterator find(const Key& key) {
        //iterator it = rbt.find(key);
        return rbt.find(key);
    }
    bool insert(const std::pair<Key, Value>& k_v) {
        return rbt.insert(k_v.first, k_v.second);
    }
    bool erase(const Key& key) {
        return rbt.erase(key);
    }
    Value& operator[](const Key& key){//重载[]符号
        //rbt.insert(make_pair(key, Value()));
        //rbt.insert(key, Value());
        //RB_tree_node<Key, Value>* temp = rbt.Find(key);
        std::shared_ptr<RB_tree_node<Key, Value>> temp = rbt.Find(key);
        return temp->value;
	    // std::pair<iterator,bool> ret = rbt.insert(make_pair(key, Value()));
		// //模板参数的V() 缺省值.
		// return *(ret.first);
	}
    int size() {
        return rbt.getSize();
    }


};
#endif
#ifndef MYSET_H
#define MYSET_H
#include "RB_tree.h"
#include "RB_tree.cpp"
template<class Key>
class mySet 
{
private:
    RB_tree<Key, Key> rbt;//set内部的红黑树
public:
    typedef typename RB_tree<Key, Key>::iterator iterator;//告诉编译器iterator是一种类型
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
    bool insert(const Key& key) {
        return rbt.insert(key, key);
    }
    bool erase(const Key& key) {
        return rbt.erase(key);
    }
    int size() {
        return rbt.getSize();
    }


};
#endif
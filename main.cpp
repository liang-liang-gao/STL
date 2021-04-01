#include "mySet.h"
#include "myMap.h"
#include<iostream>
#include<set>
using namespace std;
int main() {
    myMap<int,int> x;
    for(int i=0;i<100;i++) {
        x.insert(make_pair(i,i*2));

    }
    for(int i=0;i<100;i++) {
        cout<<x[i]<<endl;
    }
    auto rr = x.find(5);
    cout<<*rr<<endl;
    

    



    mySet<int> temp;
    temp.insert(2);
    auto p=temp.begin();
    mySet<int> temp2 = temp;
    temp.insert(10);
    temp.insert(100);
    temp.insert(500);
    temp.insert(1);
    temp.insert(1000);
    temp.insert(5000);
    for(int i=0;i<1000;i+=50) {
        temp.insert(i);
    }
    p = temp.begin();
    while(p != temp.end()) {
        cout<<*p<<endl;
        p++;
    }
    // p = temp.find(500);

    // if(p != temp.end()) {
    //     cout<<*p<<endl;
    //     temp.erase(*p);
    // } 
    // p = temp.begin();
    // while(p != temp.end()) {
    //     cout<<*p<<endl;
    //     p++;
    // }

    set<int> a;
    a.insert(3);
    //cout<<a(3)<<endl;
    myMap<int, int> w;
    w.insert(make_pair<int,int>(2,4));
    auto tt = w.find(2);
    cout<<*tt<<endl;
    myMap<int,int>::iterator ww;
    //cout<<w[2]<<endl;
    // cout<<w[1]<<endl;
    // cout<<w[0]<<endl;
    set<int> s;
    s.insert(2);
    s.insert(1);
    s.insert(5);
    auto ip = s.end();
    ip++;
    cout<<*ip<<endl;
    ip++;
    cout<<*ip<<endl;
    cout<<*ip<<endl;
    s.insert(2);
    cout<<*ip<<endl;
    s.insert(5);

    // mySet<int> m;
    // m.insert(2);
    // m.insert(1);
    // m.insert(5);
    // auto it = m.begin();
    // while(it != m.end()) {
    //     cout<<*it<<endl;
    //     it++;
    // } 
    // it = m.find(5);
    // if(it != m.end()){
    //     cout<<*it<<endl;
    // }

    // //cout<<*ip<<endl;
    // //if(ip != s.end()) cout<<"!="<<endl;
    // //ip = s.find(2)
    // //cout<<endl;
    // // ip--;
    // // cout<<*ip<<endl;
    // // ip--;
    // // cout<<*ip<<endl;
    // // ip--;
    // // cout<<*ip<<endl;        
    RB_tree<int, int> rb_tree;
    RB_tree<int, int>::iterator il = rb_tree.begin();
    cout<<*il<<endl;
    rb_tree.insert(2, 2);
    il++;
    cout<<*il<<endl;
    il++;
    cout<<*il<<endl;
    cout<<rb_tree.getSize()<<endl;
    // rb_tree.insert(5, 5);
    // rb_tree.insert(10,10);
    // rb_tree.insert(100,100);
    // rb_tree.insert(1000,1000);
    // cout<<rb_tree.check()<<endl;
    // it = rb_tree.find(100);
    // it++;
    // ++it;
    // --it;
    // it--;
    // if(it != rb_tree.end()) cout<<*it<<endl;
    // it = rb_tree.begin();
    // //it++;
    // cout<<*it<<endl;
    // while(it != rb_tree.begin()) {
    //     cout<<*it;
    //     it--;
    // }
    // //it--;
    // rb_tree.insert(100, 100);

    // it++;
    // cout<<*it<<endl;
    // rb_tree.insert(500, 500);
    // while(it != rb_tree.begin()) {
    //     cout<<*it;
    //     it--;
    // }
    // it--;
    // //一个问题：当前迭代器指向begin（），那么再--时指向哪里？
    // cout<<*it<<endl;
    // while(it != rb_tree.end()) {
    //     cout<<*it;
    //     it++;
    // }
    // rb_tree.inOrder();


    // int flag;//插入/删除操作
    // int k;
    // while(cin>>flag && flag != -1) {
    //     cin>>k;
    //     if(flag) {
    //         rb_tree.insert(k, k);
    //         it = rb_tree.begin();
    //         rb_tree.inOrder();
    //         while(it != rb_tree.end()) {
    //             cout<<*it<<" "<<endl;
    //         }
    //     }
    //     else {
    //         rb_tree.erase(k);
    //         rb_tree.inOrder();
    //         it = rb_tree.begin();
    //         while(it != rb_tree.end()) {
    //             cout<<*it<<" "<<endl;
    //         }
    //     }

        
    //     // bool p = rb_tree.find(k)->color;
    //     // if(p) cout<<"red"<<endl;
    //     // else cout<<"black"<<endl;
    // }
    // rb_tree.erase(50);
    // rb_tree.inOrder();
    // rb_tree.insert("gll", 2);
    // cout<<rb_tree.find("gll")->value<<endl;
    // rb_tree.insert("zec", 5);
    // auto p = rb_tree.find("zec");

    // cout<<p->value<<" "<<p->color<<endl;
    return 0;
}
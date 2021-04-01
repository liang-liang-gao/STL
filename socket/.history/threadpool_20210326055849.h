/*
    threadpool.h 2021/3.=/26
    功能：线程池
*/
#include <pthread.h>
#include <list>
#include <iostream>
using namespace std;
template <class T>
class threadPool {
private:
    list<T*> workqueue;//工作队列
    int max_requests;//工作队列的最大任务数
    int cur_requests;//当前工作队列中任务数量
    
    pthread_t* threads;//线程池数组
    int thread_num;//线程池的最大数量
    int cur_thread;//当前活跃线程数目
    pthread_mutex_t workqueue_lock;//互斥锁用来锁任务队列

    //工作队列的条件变量
    pthread_cond_t workqueue_not_empty;//判断工作队列不为空（任务加入队列时发出signal）
    pthread_cond_t workqueue_not_full;//判断工作队列不为满（线程从工作队列中读出数据时发出signal）
public:
    threadPool(int _thread_num, int _max_requests, T* process);
    ~threadPool();
    bool threadpool_add(void* args);//第一个参数是处理的任务，第二个参数是任务的参数
};

template <class T> 
threadPool<T>::threadPool(int _thread_num, int _max_requests,T* process ) {
    if(_thread_num <= 0 || _max_requests <= 0) {
        perror("create threadPool error:");
        exit(1);
    }
    //1.创建对应个大小的线程池，并将状态设为分离
    thread_num = _thread_num;
    threads = new pthread_t[thread_num];//开辟线程数组
    if(threads == nullptr) {
        perror("new threads error:");
        exit(1)
    }
    for(int i = 0; i < thread_num; i++) {
        if(pthread_create(threads + i, nullptr, )
    }


}
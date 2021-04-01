/*
    threadpool.h 2021/3.=/26
    功能：线程池
*/
#include <pthread.h>
#include <list>
#include <iostream>
using namespace std;
extern void process_data(int socket_fd);
void *threadPool_thread(void *threadpool);
typedef struct {
    void *(*function)(void *);//函数指针
    void *arg;//参数
}threadPool_task;//工作队列中的任务类型



class threadPool {
public:
    list<int> workqueue;//工作队列
    int max_requests;//工作队列的最大任务数
    
    pthread_t* threads;//线程池数组
    int thread_num;//线程池的最大数量
public:
    pthread_mutex_t workqueue_lock;//互斥锁用来锁任务队列

    //工作队列的条件变量
    pthread_cond_t workqueue_not_empty;//判断工作队列不为空（任务加入队列时发出signal）
    pthread_cond_t workqueue_not_full;//判断工作队列不为满（线程从工作队列中读出数据时发出signal）
    //void *threadPool_thread(void *threadPool);//
public:
    threadPool(int _thread_num, int _max_requests);
    ~threadPool() { delete []threads; }
    bool threadpool_add(int arg);//第一个参数是处理的任务，第二个参数是任务的参数
};

threadPool::threadPool(int _thread_num, int _max_requests) {
    if(_thread_num <= 0 || _max_requests <= 0) {
        perror("create threadPool error:");
        exit(1);
    }
    //1.初始化互斥锁和条件变量
    if(pthread_mutex_init(&(workqueue_lock), nullptr) != 0
        ||pthread_cond_init(&(workqueue_not_empty), nullptr) != 0
        ||pthread_cond_init(&(workqueue_not_full), nullptr) != 0) {
        delete []threads;
        perror("init the lock or cond error:");
        exit(1);
    }
    //2.创建对应个大小的线程池，并将状态设为分离
    thread_num = _thread_num;
    threads = new pthread_t[thread_num];//开辟线程数组
    if(threads == nullptr) {
        perror("new threads error:");
        exit(1);
    }
    for(int i = 0; i < thread_num; i++) {
        if(pthread_create(threads + i, nullptr, threadPool_thread, (void *)this) != 0) {
            delete []threads;
            perror("pthread_create error:");
            exit(1);
        }
        cout<<"start thread:"<<threads[i]<<endl;
        if(pthread_detach(threads[i]) != 0) {
            delete []threads;
            perror("pthread_detech error:");
            exit(1);
        }
    }
    max_requests = _max_requests;
}

void threadPool_create(int _thread_num, int _max_requests) {
    //threadPool *pool = new threadPool(0, 0);
    // if(_thread_num <= 0 || _max_requests <= 0) {
    //     perror("create threadPool error:");
    //     exit(1);
    // }

    // //1.初始化互斥锁和条件变量
    // if(pthread_mutex_init(&(workqueue_lock), nullptr) != 0
    //     ||pthread_cond_init(&(workqueue_not_empty), nullptr) != 0
    //     ||pthread_cond_init(&(workqueue_not_full), nullptr) != 0) {
    //     //delete []threads;
    //     perror("init the lock or cond error:");
    //     exit(1);
    // }
    // //2.创建对应个大小的线程池，并将状态设为分离
    // thread_num = _thread_num;
    // threads = new pthread_t[thread_num];//开辟线程数组
    // if(threads == nullptr) {
    //     perror("new threads error:");
    //     exit(1);
    // }
    // for(int i = 0; i < thread_num; i++) {
    //     if(pthread_create(&(threads[i]), nullptr, threadPool_thread, nullptr) != 0) {
    //         //delete []pool->threads;
    //         perror("pthread_create error:");
    //         exit(1);
    //     }
    //     cout<<"start thread:"<<threads[i]<<endl;
    //     // if(pthread_detach(threads[i]) != 0) {
    //     //     //delete []threads;
    //     //     perror("pthread_detech error:");
    //     //     exit(1);
    //     // }
    // }


    // max_requests = _max_requests;
    // cur_thread = 0;
    // cur_requests = 0;
    //return pool;
 }
 //线程回调函数，即线程需要执行的任务，参数为线程池
//线程回调函数，为啥必须在外面？？？因为回调函数的类型不能是任何类的类型
void *threadPool_thread(void *threadpool) {
    threadPool* pool = (threadPool *)threadpool;
    threadPool_task task;
    while(1) {
        //利用条件变量上锁，阻塞
        pthread_mutex_lock(&(pool->workqueue_lock));
        while(pool->workqueue.size() == 0) {
            //cout<<"thread "<<pthread_self()<<" is waiting"<<endl;
            pthread_cond_wait(&(pool->workqueue_not_empty), &(pool->workqueue_lock));
        }
        //从队列中取出数据并处理
        // task.function = pool->workqueue.front()->function;
        // task.arg = pool->workqueue.front()->arg;
        int fd = pool->workqueue.front();
        
        pool->workqueue.pop_front();
        pool->cur_requests--;
        //通知可以有新的任务
        pthread_cond_broadcast(&(pool->workqueue_not_full));
        pthread_mutex_unlock(&(pool->workqueue_lock));
        cout<<"thread "<<pthread_self()<<" is processing data!"<<endl;
        //执行回调函数
        process_data(fd);
        //((task.function))(task.arg);
    }
}
//向池中加一个任务
bool threadPool::threadpool_add(int arg) {
    //threadPool* pool;
    pthread_mutex_lock(&(workqueue_lock));//访问公共前先加锁
    //队列满则调用wait阻塞
	while(workqueue.size() == max_requests) {
        cout<<"waiting..."<<endl;
		pthread_cond_wait(&(workqueue_not_full), &(workqueue_lock));
    }
    //插入队列中
    // threadPool_task* task;
    // task->function = function;
    // task->arg = arg;
    workqueue.emplace_back(arg);
    cur_requests++;
    //唤醒全部线程，争夺资源
    pthread_cond_broadcast(&(workqueue_not_empty));
    pthread_mutex_unlock(&(workqueue_lock));
    return true;
}



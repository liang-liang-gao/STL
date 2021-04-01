/*
    threadpool.h 2021/3.=/26
    功能：线程池
*/
#include <pthread.h>
#include <list>
#include <iostream>
#include <vector>
using namespace std;
extern void process_data(int socket_fd);
// extern struct Fds;
void *threadPool_thread(void *threadpool);
//标记函数和参数的结构体
typedef struct {
    void (*function)(void *);//函数指针,从左到右分别是返回类型，函数指针标志，参数
    void * arg;//参数
}threadPool_task;//工作队列中的任务类型


//线程池类
class threadPool {
public:
    list<threadPool_task*> workqueue;//工作队列
    int max_requests;//工作队列的最大任务数
    
    //pthread_t* threads;//线程池数组
    vector<pthread_t*> threads;
    int max_threads;//线程池的最大数量
    int min_threads;//线程池的最小线程数目
    int cur_threads;//当前线程数目
public:
    pthread_mutex_t workqueue_lock;//互斥锁用来锁任务队列

    //工作队列的条件变量
    pthread_cond_t workqueue_not_empty;//判断工作队列不为空（任务加入队列时发出signal）
    pthread_cond_t workqueue_not_full;//判断工作队列不为满（线程从工作队列中读出数据时发出signal）
    //void *threadPool_thread(void *threadPool);//
public:
    threadPool(int _min_threads, int _max_threads, int _max_requests);
    //~threadPool() { delete []threads; }
    bool threadpool_add(void (*function)(void *), void * arg);//第一个参数是处理的任务(函数指针)，第二个参数是任务的参数
};

threadPool::threadPool(int _min_threads, int _max_threads, int _max_requests) {
    if(_min_threads <= 0 ||_max_threads <=0 || _max_requests <= 0) {
        perror("create threadPool error:");
        exit(1);
    }
    //1.初始化互斥锁和条件变量
    if(pthread_mutex_init(&(workqueue_lock), nullptr) != 0
        ||pthread_cond_init(&(workqueue_not_empty), nullptr) != 0
        ||pthread_cond_init(&(workqueue_not_full), nullptr) != 0) {
        //delete []threads;
        perror("init the lock or cond error:");
        exit(1);
    }
    //2.创建对应个大小的线程池，并将状态设为分离
    min_threads = _min_threads;
    max_threads = _max_threads;
    pthread_t *_threads = new pthread_t[min_threads]; 
    //= new pthread_t[min_threads];
    //threads = new pthread_t[thread_num];//开辟线程数组
    

    if(_threads == nullptr) {
        perror("new threads error:");
        exit(1);
    }
    
    for(int i = 0; i < min_threads; i++) {
        threads.push_back(&_threads[i]);
        if(pthread_create(threads[i], nullptr, threadPool_thread, (void *)this) != 0) {
            //delete []threads;
            perror("pthread_create error:");
            exit(1);
        }
        cout<<"start thread:"<<threads[i]<<endl;
        if(pthread_detach(*threads[i]) != 0) {//线程结束后自动释放所有资源
            //elete []threads;
            perror("pthread_detech error:");
            exit(1);
        }
    }
    max_requests = _max_requests;
    cur_threads = 0;
}

//void threadPool_create(int _thread_num, int _max_requests) {
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
 //}
 //线程回调函数，即线程需要执行的任务，参数为线程池
//线程回调函数，为啥必须在外面？？？因为回调函数的类型不能是任何类的类型
void *threadPool_thread(void *threadpool) {
    threadPool* pool = (threadPool *)threadpool;
    threadPool_task task;//不能写成指针形式，否则会导致指向的内存被析构
    while(1) {//重复利用线程
        //利用条件变量上锁，阻塞
        pthread_mutex_lock(&(pool->workqueue_lock));
        while(pool->workqueue.size() == 0) {
            //cout<<"thread "<<pthread_self()<<" is waiting"<<endl;
            pthread_cond_wait(&(pool->workqueue_not_empty), &(pool->workqueue_lock));
        }
        //从队列中取出数据并处理
        task.function = pool->workqueue.front()->function;
        task.arg = pool->workqueue.front()->arg;
        //int fd = pool->workqueue.front();
        
        pool->workqueue.pop_front();
        //通知可以有新的任务
        pthread_cond_broadcast(&(pool->workqueue_not_full));
        pthread_mutex_unlock(&(pool->workqueue_lock));
        cout<<"thread "<<pthread_self()<<" is processing data!"<<endl;
        //执行回调函数
        pool->cur_threads++;
        // int	sock_fd = ((struct Fds *)task.arg)->sock_fd;
        // int event_fd = ((struct Fds *)task.arg)->epoll_fd;    
        (task.function)(task.arg);
        //process_data(fd);
        //((task.function))(task.arg);
    }
}
//向池中加一个任务
bool threadPool::threadpool_add(void (*function)(void *), void * arg) {
    //如果超过当前最小线程数目，但没有达到最大线程池数目，则可以新建线程，提高效率
    // int	sock_fd = ((struct Fds *)arg)->sock_fd;
    // int event_fd = ((struct Fds *)arg)->epoll_fd;    
    if(cur_threads >= min_threads && cur_threads < max_threads) {
        pthread_t *new_thread = new pthread_t;
        threads.emplace_back(new_thread);
        //cur_threads++;
        cout<<"have created a new pthread "<<pthread_self()<<endl;
        if(pthread_create(threads[threads.size()-1], nullptr, threadPool_thread, (void *)this) != 0) {
            //delete []threads;
            perror("pthread_create error:");
            exit(1);
        }
    }
    pthread_mutex_lock(&(workqueue_lock));//访问公共前先加锁
    //队列满则调用wait阻塞
	while(workqueue.size() == max_requests) {
        cout<<"waiting..."<<endl;
		pthread_cond_wait(&(workqueue_not_full), &(workqueue_lock));
    }
    //任务和参数插入队列中
    threadPool_task* task = new threadPool_task;
    task->function = function;
    task->arg = arg;
    workqueue.emplace_back(task);
    //唤醒全部线程，争夺资源
    pthread_cond_broadcast(&(workqueue_not_empty));
    pthread_mutex_unlock(&(workqueue_lock));
    return true;
}



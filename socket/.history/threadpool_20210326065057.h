/*
    threadpool.h 2021/3.=/26
    功能：线程池
*/
#include <pthread.h>
#include <list>
#include <iostream>
using namespace std;
typedef struct {
    void *(*function)(void *);//函数指针
    void *arg;//参数
}threadPool_task;//工作队列中的任务类型

class threadPool {
private:
    list<threadPool_task*> workqueue;//工作队列
    int max_requests;//工作队列的最大任务数
    int cur_requests;//当前工作队列中任务数量
    
    pthread_t* threads;//线程池数组
    int thread_num;//线程池的最大数量
    int cur_thread;//当前活跃线程数目
public:
    pthread_mutex_t workqueue_lock;//互斥锁用来锁任务队列

    //工作队列的条件变量
    pthread_cond_t workqueue_not_empty;//判断工作队列不为空（任务加入队列时发出signal）
    pthread_cond_t workqueue_not_full;//判断工作队列不为满（线程从工作队列中读出数据时发出signal）
    //void *threadPool_thread(void *threadPool);//线程回调函数，即线程需要执行的任务，参数为线程池
public:
    threadPool(int _thread_num, int _max_requests);
    int workqueue_size() { return cur_requests; }
    ~threadPool();
    bool threadpool_add(void*(*function)(void *args), void* args);//第一个参数是处理的任务，第二个参数是任务的参数
};

threadPool::threadPool(int _thread_num, int _max_requests) {
    if(_thread_num <= 0 || _max_requests <= 0) {
        perror("create threadPool error:");
        exit(1);
    }
    //1.创建对应个大小的线程池，并将状态设为分离
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
    //2.初始化互斥锁和条件变量
    if(pthread_mutex_init(&(workqueue_lock), nullptr) != 0
        ||pthread_cond_init(&(workqueue_not_empty), nullptr) != 0
        ||pthread_cond_init(&(workqueue_not_full), nullptr) != 0) {
        delete []threads;
        perror("init the lock or cond error:");
        exit(1);
    }

    max_requests = 0;
    cur_thread = 0;
    cur_requests = 0;
}

// threadPool *threadPool_create(int _thread_num, int _max_requests) {
//     threadPool *pool = nullptr;
//     if(_thread_num <= 0 || _max_requests <= 0) {
//         perror("create threadPool error:");
//         exit(1);
//     }
//     //1.创建对应个大小的线程池，并将状态设为分离
//     thread_num = _thread_num;
//     threads = new pthread_t[thread_num];//开辟线程数组
//     if(threads == nullptr) {
//         perror("new threads error:");
//         exit(1);
//     }
//     for(int i = 0; i < thread_num; i++) {
//         if(pthread_create(threads + i, nullptr, threadPool_thread, (void *)this) != 0) {
//             delete []threads;
//             perror("pthread_create error:");
//             exit(1);
//         }
//         cout<<"start thread:"<<threads[i]<<endl;
//         if(pthread_detach(threads[i]) != 0) {
//             delete []threads;
//             perror("pthread_detech error:");
//             exit(1);
//         }
//     }
//     //2.初始化互斥锁和条件变量
//     if(pthread_mutex_init(&(workqueue_lock), nullptr) != 0
//         ||pthread_cond_init(&(workqueue_not_empty), nullptr) != 0
//         ||pthread_cond_init(&(workqueue_not_full), nullptr) != 0) {
//         delete []threads;
//         perror("init the lock or cond error:");
//         exit(1);
//     }

//     max_requests = 0;
//     cur_thread = 0;
//     cur_requests = 0;
//  }
//线程回调函数，为啥必须在外面？？？
void *threadPool_thread(void *threadpool) {
    threadPool* pool = (threadPool *)threadpool;
    threadPool_task task;
    while(1) {
        //利用条件变量上锁，阻塞
        pthread_mutex_lock(&(pool->workqueue_lock));
        while(pool->workqueue_size() == 0) {
            cout<<"thread "<<pthread_self()<<"is waiting"<<endl;
            pthread_cond_wait(&(pool->workqueue_not_empty), &(pool->workqueue_lock));
        }
        //解锁，可以从任务队列中拿数据
        111
    }
}
//向池中加一个任务
bool threadPool::threadpool_add(void*(*function)(void *args), void *args) {
    pthread_mutex_lock(&(workqueue_lock));//访问公共前先加锁
    //队列满则调用wait阻塞
	while(cur_requests == max_requests) {
		pthread_cond_wait(&(workqueue_not_full), &(workqueue_lock));
    }
    //唤醒全部线程
    pthread_cond_broadcast(&(pool->queue_not_empty));
    pthread_mutex_unlock(&(pool->lock));
    return 0;
	}

    //清空工作线程 调用的回调函数 的参数arg
    if(pool->task_queue[pool->queue_rear] != NULL)
    {
    pool->task_queue[pool->queue_rear].arg = NULL;
    }

//添加任务到任务队列里
	pool->task_queue[pool->queue_rear].function = function;
	pool->task_queue[pool->queue_rear].arg = arg;
	pool->queue_rear = (pool->queue_rear +1) % pool->queue_max_size；//环形 后移
	pool->queue_size++;//task_queue中实际任务数+1

//添加完任务后，队列不为空，唤醒线程池中 等待处理任务的线程
	pthread_cond_signal(&(pool->queue_not_empty));
	pthread_mutex_unlock(&(pool->lock));
}


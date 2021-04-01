#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
using namespace std;
pthread_barrier_t barrier;

void *go(void *arg){
    sleep (rand () % 10);
    printf("%lu is arrived.\n", pthread_self());
    pthread_barrier_wait(&barrier);
    printf("%lu go shopping...\n", pthread_self());
}

int main() {
    pthread_barrier_init(&barrier, NULL, 5);

    pthread_t t1,t2,t3;
    pthread_create(&t1, NULL, &go, NULL);
    pthread_create(&t2, NULL, &go, NULL);
    pthread_create(&t3, NULL, &go, NULL);

    pthread_join(t1, NULL);
    return 0;
}


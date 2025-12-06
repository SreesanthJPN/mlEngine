#define _XOPEN_SOURCE 600 
#include<pthread.h>
#include "createTiles.h"

typedef struct threadPool{
    float** matA;
    float** resMat;
    pthread_t* tPool;
    tile* tiles;
    int resultRows;
    int resultCols;
    int nTasks;
    int start;
    int stop;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_barrier_t barrier;
}threadPool;

typedef struct workerArgs{
    threadPool* tPool;

}workerArgs;

void* worker(void* args){
    threadPool* tPool = (threadPool*)args;

    while(1){    
    pthread_mutex_lock(&tPool->lock);
    while(!tPool->start && !tPool->stop){
        pthread_cond_wait(&tPool->cond, &tPool->lock);
    }
    if(!tPool->stop)
    {
        pthread_mutex_unlock(&tPool->lock);
        return NULL;
    }
    pthread_mutex_unlock(&tPool->lock);
}
}
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

typedef struct threadPool {
    long nworkerThreads;
    pthread_t *tPool;
    float **A;
    float ***tiles;        
    float **resultMat;

    int rowsA;
    int colsA;

    int tileSize;
    int tileCount;

    pthread_mutex_t taskLock;
    pthread_cond_t taskCond;
    pthread_barrier_t barrier;

    int taskReady;
    int shutdown;
} threadPool;

void* worker(void* arg);

float*** tileMatrix(float **MT, int rows, int cols, int tileSize, int *tileCountOut);

void createThreadPool(threadPool* pool, int tileSize);
#endif
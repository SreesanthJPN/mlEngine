#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

typedef struct funcArgs{
    float* row1;
    int rows;
    float** mat2;
    int cols;
    int resultRow;
}funcArgs;

typedef struct taskQueue{
    funcArgs* tQueue;
    float** mat1;
    int cols;
    int mat1Rows;
    float** mat2;
    int mat2Rows;
    int taskCount;
}taskQueue;

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
float* task(float* row1, int cols, float** mat2, int rows);

funcArgs createArgs(float* r1, int r, float** m2, int c, int resultRow);

taskQueue* createTaskQueue(float** mat1, int m1Rows, int cols, float** mat2, int m2Rows);

void* worker(void* arg);

float*** tileMatrix(float **MT, int rows, int cols, int tileSize, int *tileCountOut);

void createThreadPool(threadPool* pool, int tileSize);
#endif
#define _XOPEN_SOURCE 600 

#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<memory.h>
#include <bits/pthreadtypes.h>

float* task(float* row1, int cols, float** mat2, int rows){
    float *result = (float*)malloc(rows * sizeof(float));
    for(int i = 0; i < rows; i++){
        float temp = (float)0;
        for(int j = 0; j < cols; j++){
            temp += row1[j] * mat2[i][j];
        }
    result[i] = temp;
    }
    return result;
}

float*** tileMatrix(float **MT, int rows, int cols, int tileSize, int *tileCountOut) {

    int tileCount = (cols + tileSize - 1) / tileSize;
    *tileCountOut = tileCount;

    float ***tiles = malloc(tileCount * sizeof(float**));

    for (int t = 0; t < tileCount; t++) {
        int start = t * tileSize;
        int end   = (start + tileSize < cols) ? start + tileSize : cols;

        int width = end - start;

        tiles[t] = malloc(rows * sizeof(float*));
        for (int i = 0; i < rows; i++)
            tiles[t][i] = &MT[i][start];  // pointer view
    }
    return tiles;
}


typedef struct funcArgs{
    float* row1;
    int rows;
    float** mat2;
    int cols;
    int resultRow;
}funcArgs;

funcArgs createArgs(float* r1, int r, float** m2, int c, int resultRow) {
    funcArgs a;
    a.row1 = r1;
    a.rows = r;
    a.mat2 = m2;
    a.cols = c;
    a.resultRow = resultRow;
    return a;
}


typedef struct taskQueue{
    funcArgs* tQueue;
    float** mat1;
    int cols;
    int mat1Rows;
    float** mat2;
    int mat2Rows;
    int taskCount;
}taskQueue;

taskQueue* createTaskQueue(float** mat1, int m1Rows, int cols, float** mat2, int m2Rows){
    taskQueue* tempQueue = (taskQueue*)malloc(sizeof(taskQueue));
    tempQueue->mat1 = mat1;
    tempQueue->mat2 = mat2;
    tempQueue->mat1Rows = m1Rows;
    tempQueue->mat2Rows = m2Rows;
    tempQueue->taskCount = m1Rows;

    tempQueue->tQueue = (funcArgs*)malloc(tempQueue->taskCount * sizeof(funcArgs));

    for(int i = 0; i < m1Rows; i++){
            funcArgs a = createArgs(mat1[i], m2Rows, mat2, cols, i);  
            tempQueue->tQueue[i] = a;   
    }
    return tempQueue;
}

typedef struct threadPool {
    long nworkerThreads;
    pthread_t *tPool;

    // Matrix pointers
    float **A;
    float ***tiles;         // array of tiles of Mᵀ
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

void* worker(void* arg){
    threadPool* pool = (threadPool*)arg;

    while (1) {

        pthread_mutex_lock(&pool->taskLock);

        while (!pool->taskReady && !pool->shutdown)
            pthread_cond_wait(&pool->taskCond, &pool->taskLock);

        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->taskLock);
            pthread_exit(NULL);
        }

        pthread_mutex_unlock(&pool->taskLock);

        // Tile phases
        for (int t = 0; t < pool->tileCount; t++) {

            int start = t * pool->tileSize;
            int end   = (start + pool->tileSize < pool->colsA)
                        ? start + pool->tileSize
                        : pool->colsA;

            for (int r = 0; r < pool->rowsA; r++) {

                float sum = 0;
                for (int k = start; k < end; k++)
                    sum += pool->A[r][k] * pool->tiles[t][r][k - start];

                pool->resultMat[r][t] = sum;
            }

            pthread_barrier_wait(&pool->barrier);
        }

        pthread_mutex_lock(&pool->taskLock);
        pool->taskReady = 0;
        pthread_mutex_unlock(&pool->taskLock);
    }
}


void createThreadPool(threadPool* pool, int tileSize){
    pool->nworkerThreads = sysconf(_SC_NPROCESSORS_ONLN);
    pool->tPool = malloc(pool->nworkerThreads * sizeof(pthread_t));

    pool->tileSize = tileSize;
    pool->taskReady = 0;
    pool->shutdown = 0;

    pthread_mutex_init(&pool->taskLock, NULL);
    pthread_cond_init(&pool->taskCond, NULL);
    pthread_barrier_init(&pool->barrier, NULL, pool->nworkerThreads + 1);

    for(int i = 0; i < pool->nworkerThreads; i++)
        pthread_create(&pool->tPool[i], NULL, worker, pool);
}


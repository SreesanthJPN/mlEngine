#define _XOPEN_SOURCE 600 

#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<memory.h>
#include <bits/pthreadtypes.h>

float*** tileMatrix(float **MT, int rows, int cols, int tileSize, int *tileCountOut) {

    int tileCount = (rows + tileSize - 1) / tileSize;
    *tileCountOut = tileCount;

    float ***tiles = malloc(tileCount * sizeof(float**));

    for (int t = 0; t < tileCount; t++) {

        int rowStart = t * tileSize;
        int rowEnd = (rowStart + tileSize < rows) ? rowStart + tileSize : rows;

        int height = rowEnd - rowStart;

        // allocate array-of-rows for this tile
        tiles[t] = malloc(height * sizeof(float*));

        for (int i = 0; i < height; i++)
            tiles[t][i] = MT[rowStart + i];   // row pointers
    }

    return tiles;
}


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

typedef struct {
    threadPool* pool;
    long tid;
} threadArg;


void* worker(void* arg) {
    threadArg* a = (threadArg*)arg;
    threadPool* pool = a->pool;
    long tid = a->tid;
    free(a);

    int M = pool->rowsA;
    int K = pool->colsA;
    int rowsPerThread = (M + pool->nworkerThreads - 1) / pool->nworkerThreads;

    int rStart = tid * rowsPerThread;
    int rEnd   = (rStart + rowsPerThread < M) ? (rStart + rowsPerThread) : M;

    while (1) {
        pthread_mutex_lock(&pool->taskLock);
        while (!pool->taskReady && !pool->shutdown)
            pthread_cond_wait(&pool->taskCond, &pool->taskLock);
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->taskLock);
            return NULL;
        }
        pthread_mutex_unlock(&pool->taskLock);

        // process all tiles
        for (int t = 0; t < pool->tileCount; t++) {
            float** tileRows = pool->tiles[t];
            int rowsInTile = (t == pool->tileCount - 1)
                ? pool->rowsA - t * pool->tileSize
                : pool->tileSize;

            int colOffset = t * pool->tileSize;

            for (int r = rStart; r < rEnd; r++) {
                float* rowA = pool->A[r];
                for (int i = 0; i < rowsInTile; i++) {
                    float* rowBT = tileRows[i];
                    float sum = 0;
                    for (int k = 0; k < K; k++)
                        sum += rowA[k] * rowBT[k];
                    pool->resultMat[r][colOffset + i] = sum;
                }
            }
        }

        pthread_barrier_wait(&pool->barrier);

        pthread_mutex_lock(&pool->taskLock);
        pool->taskReady = 0;
        pthread_mutex_unlock(&pool->taskLock);
    }
}


void createThreadPool(threadPool* pool, int tileSize) {
    pool->nworkerThreads = sysconf(_SC_NPROCESSORS_ONLN);
    pool->tPool = malloc(pool->nworkerThreads * sizeof(pthread_t));

    pool->tileSize = tileSize;
    pool->taskReady = 0;
    pool->shutdown = 0;

    pthread_mutex_init(&pool->taskLock, NULL);
    pthread_cond_init(&pool->taskCond, NULL);
    pthread_barrier_init(&pool->barrier, NULL, pool->nworkerThreads + 1);

    for (int i = 0; i < pool->nworkerThreads; i++) {
        threadArg* a = malloc(sizeof(threadArg));
        a->pool = pool;
        a->tid = i;
        pthread_create(&pool->tPool[i], NULL, worker, a);
    }
}

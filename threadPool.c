#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<memory.h>

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

typedef struct threadPool{
    long nworkerThreads;
    pthread_t *tPool; // Allocate size based on the no of workerThreads
    taskQueue* tasks;
    pthread_mutex_t taskLock;
    pthread_cond_t taskCond;
    int nextTask;
    int taskReady;
    int shutdown;
    float** resultMat;
}threadPool;

void* worker(void* arg){
    threadPool* tpool = (threadPool*)arg;

    while (1) {

        pthread_mutex_lock(&tpool->taskLock);

        while (tpool->taskReady == 0 && tpool->shutdown == 0) {
            pthread_cond_wait(&tpool->taskCond, &tpool->taskLock);
        }

        if (tpool->shutdown == 1) {
            pthread_mutex_unlock(&tpool->taskLock);
            pthread_exit(NULL);
        }

        if (tpool->nextTask >= tpool->tasks->taskCount) {
            tpool->taskReady = 0;
            pthread_mutex_unlock(&tpool->taskLock);
            continue;
        }

        int myTaskIndex = tpool->nextTask;
        tpool->nextTask++;

        pthread_mutex_unlock(&tpool->taskLock);

        funcArgs args = tpool->tasks->tQueue[myTaskIndex];

        float* row = task(
            args.row1,
            args.cols,
            args.mat2,
            args.rows
        );
        tpool->resultMat[args.resultRow] = row;
    }

    return NULL;
}


void createThreadPool(threadPool* pool){
    pool->nworkerThreads = sysconf(_SC_NPROCESSORS_ONLN);
    pool->tPool = (pthread_t*)malloc(pool->nworkerThreads * sizeof(pthread_t));
    pool->tasks = NULL;
    pool->resultMat = NULL;
    pool->nextTask = 0;
    pool->taskReady = 0;
    pool->shutdown = 0;

    pthread_mutex_init(&pool->taskLock, NULL);
    pthread_cond_init(&pool->taskCond, NULL);
    for(int i = 0; i < (int)pool->nworkerThreads; i++){
        pthread_create(&pool->tPool[i], NULL, worker, pool);
    }
}

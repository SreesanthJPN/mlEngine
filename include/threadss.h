#ifndef threadss
#define threadss


#ifdef __cplusplus
extern "C" {
#endif

#include "2dTiles.h"
#include<pthread.h>




typedef struct taskArgs{
    tileData tileA;
    tileData tileB;
}taskArgs;

typedef struct taskQueue{
     taskArgs* tks;
     int nTasks;
}taskQueue;

taskQueue* createTaskQueue(tile* matATiles, tile* matBTiles);
double** createMatrix(int rows, int cols);
double** createMatrix(int rows, int cols);
typedef struct threadPool {
    double** matA;
    double** matB;
    double** resMat;
    pthread_t* tPool;
    taskQueue* tQ;

    int currentTask;
    int activeTasks;          
    int start;
    int stop;

    int nThreads;
    pthread_mutex_t lock;
    pthread_cond_t cond;     
    pthread_cond_t done;      
} threadPool;


typedef struct workerArgs{
    threadPool* tPool;

}workerArgs;

void* worker(void* args);
threadPool* createThreadPool();
void startMult(threadPool* tP, taskQueue* tQ,double** matA, double** matB, double** res);
double** createMatrix(int rows, int cols);


#ifdef __cplusplus
}
#endif

#endif 
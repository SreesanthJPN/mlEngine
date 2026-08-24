#include<pthread.h>
#include<sched.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include "2dTiles.h"

double getRandomFloat() {
    return (double)rand() / (double)RAND_MAX;
}

typedef struct taskArgs{
    tileData tileA;
    tileData tileB;
}taskArgs;

typedef struct taskQueue{
     taskArgs* tks;
     int nTasks;
}taskQueue;

taskQueue* createTaskQueue(tile* matATiles, tile* matBTiles){
    taskQueue* tQ = (taskQueue*)malloc(sizeof(taskQueue));

    int count = 0;
    for(int i = 0; i < matBTiles->nTiles; i++){
        for(int j = 0; j < matATiles->nTiles; j++){
            if(matATiles->tD[j].colStart == matBTiles->tD[i].colStart && matATiles->tD[j].colEnd == matBTiles->tD[i].colEnd){
                count += 1;
            }
        }
    }
    tQ->nTasks = count;

    tQ->tks = (taskArgs*)malloc((size_t)count * sizeof(taskArgs));
    int c = 0;
    for(int i = 0; i < matBTiles->nTiles; i++){
        for(int j = 0; j < matATiles->nTiles; j++){
            if(matATiles->tD[j].colStart == matBTiles->tD[i].colStart && matATiles->tD[j].colEnd == matBTiles->tD[i].colEnd){
                tQ->tks[c].tileA = matATiles->tD[j];
                tQ->tks[c].tileB = matBTiles->tD[i];
                c += 1;
            }
        }
    }
    return tQ;
}

void freeMatrix(double** mat, int rows) {
    for (int i = 0; i < rows; i++) {
        free(mat[i]);
    }
    free(mat);
}

double** createMatrix(int rows, int cols) {


    double** arr = (double**)malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++) {
        arr[i] = malloc(cols * sizeof(double));
        for (int j = 0; j < cols; j++)
            arr[i][j] = (double)getRandomFloat()*0.001f;
    }
    return arr;
}

double** createzMatrix(int rows, int cols) {


    double** arr = malloc(rows * sizeof(double*));

    for (int i = 0; i < rows; i++) {
        arr[i] = malloc(cols * sizeof(double));
        for (int j = 0; j < cols; j++)
            arr[i][j] = (double)0;
    }
    return arr;
}


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

void* worker(void* args)
{
    threadPool* tPool = (threadPool*)args;

    while (1) {
        pthread_mutex_lock(&tPool->lock);
        while (!tPool->start && !tPool->stop) {
            pthread_cond_wait(&tPool->cond, &tPool->lock);
        }
        if (tPool->stop) {
            pthread_mutex_unlock(&tPool->lock);
            return NULL;
        }
        if (tPool->currentTask > tPool->tQ->nTasks) {
            pthread_mutex_unlock(&tPool->lock);
            continue;
        }
        taskArgs currArgs = tPool->tQ->tks[tPool->currentTask - 1]; 
        tPool->currentTask++;
        pthread_mutex_unlock(&tPool->lock);

        for (int i = currArgs.tileA.rowStart; i < currArgs.tileA.rowEnd; i++) {
            for (int j = currArgs.tileB.rowStart; j < currArgs.tileB.rowEnd; j++) {
                double sum = 0.0;
                for (int k = currArgs.tileA.colStart; k < currArgs.tileA.colEnd; k++)
                    sum += tPool->matA[i][k] * tPool->matB[j][k];
                tPool->resMat[i][j] += sum;
            }
        }
        pthread_mutex_lock(&tPool->lock);
        tPool->activeTasks--;
        if (tPool->activeTasks == 0 && tPool->currentTask > tPool->tQ->nTasks) {
            tPool->start = 0;
            pthread_cond_signal(&tPool->done);
        }
        pthread_mutex_unlock(&tPool->lock);
    }
}



threadPool* createThreadPool()
{
    threadPool* tP = malloc(sizeof(threadPool));
    tP->nThreads = sysconf(_SC_NPROCESSORS_ONLN);
    tP->tPool = malloc(tP->nThreads * sizeof(pthread_t));
    tP->start = 0;
    tP->stop = 0;
    tP->currentTask = 1;
    tP->activeTasks = 0;
    pthread_mutex_init(&tP->lock, NULL);
    pthread_cond_init(&tP->cond, NULL);
    pthread_cond_init(&tP->done, NULL);
    for (int i = 0; i < tP->nThreads; i++) {
        pthread_create(&tP->tPool[i], NULL, worker, tP);
    }
    return tP;
}

void startMult(threadPool* tP, taskQueue* tQ,
               double** matA, double** matB, double** res)
{
    pthread_mutex_lock(&tP->lock);
    tP->matA = matA;
    tP->matB = matB;
    tP->resMat = res;
    tP->tQ = tQ;
    tP->currentTask = 1;         
    tP->activeTasks = tQ->nTasks;
    tP->start = 1;
    tP->stop = 0;
    pthread_cond_broadcast(&tP->cond);
    while (tP->start) {
        pthread_cond_wait(&tP->done, &tP->lock);
    }
    pthread_mutex_unlock(&tP->lock);
}

void deleteMultPool(threadPool* tP){
    pthread_mutex_lock(&tP->lock);
    tP->stop = 1;
    pthread_cond_broadcast(&tP->cond);
    pthread_mutex_unlock(&tP->lock);
    for (int i = 0; i < tP->nThreads; i++) {
        pthread_join(tP->tPool[i], NULL);
    }
    pthread_mutex_destroy(&tP->lock);
    pthread_cond_destroy(&tP->cond);
    pthread_cond_destroy(&tP->done);
    free(tP->tPool);
    free(tP);
}


// int main(void){
//     double** matA = createMatrix(1000, 2000);
//     double** matB = createMatrix(8000, 2000);
//     double** res  = createzMatrix(1000, 8000);

//     tile* tilesA = createTiles(1000, 2000);
//     tile* tilesB = createTiles(8000, 2000);

//     taskQueue* tQ = createTaskQueue(tilesA, tilesB);
//     threadPool* tp =  createThreadPool();
//     startMult(tp, tQ, matA, matB, res);
//             printf("\n");

//     for(int i =900; i < 901; i++){
//         for(int j = 60; j < 70; j++){
//             printf(" %f ", res[i][j]);
//         }
//         printf("\n");
//     }
// }

#include<pthread.h>
#include<unistd.h>
#include<malloc.h>
#include "2dTiles.h"

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

tQ->nTasks = 0;
    int c = 1;
    for(int i = 0; i < matBTiles->nTiles; i++){
        for(int j = 0; j < matATiles->nTiles; j++){
            taskArgs tArg;
            if(matATiles->tD[j].colStart == matBTiles->tD[i].colStart && matATiles->tD[j].colEnd == matBTiles->tD[i].colEnd){
            c += 1;
        }    
        }
    }
    tQ->nTasks = c;
    
        tQ->tks = (taskArgs*)malloc(c* sizeof(taskArgs));
        c = 0;
        for(int i = 0; i < matBTiles->nTiles; i++){
            for(int j = 0; j < matATiles->nTiles; j++){
            taskArgs tArg;
            if(matATiles->tD[j].colStart == matBTiles->tD[i].colStart && matATiles->tD[j].colEnd == matBTiles->tD[i].colEnd){
            c += 1;
            tArg.tileA = matATiles->tD[j];
            tArg.tileB = matBTiles->tD[i];
            tQ->tks[c] = tArg;
        }  
        }
    }
    return tQ;
}

double** createMatrix(int rows, int cols) {


    double** arr = malloc(rows * sizeof(double*));

    for (int i = 0; i < rows; i++) {
        arr[i] = malloc(cols * sizeof(double));
        for (int j = 0; j < cols; j++)
            arr[i][j] = (double)1;
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
        if (tPool->currentTask >= tPool->tQ->nTasks) {
            pthread_mutex_unlock(&tPool->lock);
            continue;
        }
        taskArgs currArgs = tPool->tQ->tks[tPool->currentTask++];
        for (int i = currArgs.tileA.rowStart; i < currArgs.tileA.rowEnd; i++) {
            for (int j = currArgs.tileB.rowStart; j < currArgs.tileB.rowEnd; j++) {
                double sum = 0.0;
                for (int k = currArgs.tileA.colStart; k < currArgs.tileA.colEnd; k++) {
                     sum += tPool->matA[i][k] * tPool->matB[j][k];
                 }
                tPool->resMat[i][j] += sum;

            }
        }

        tPool->activeTasks--;
        if (tPool->activeTasks == 0 &&
            tPool->currentTask >= tPool->tQ->nTasks) {

            tPool->start = 0;
            pthread_cond_signal(&tPool->done);
        }

        pthread_mutex_unlock(&tPool->lock);
    }
}



void createThreadPool(threadPool* tP)
{
    printf("creating...");
    tP->nThreads = sysconf(_SC_NPROCESSORS_ONLN);
    tP->tPool = malloc(tP->nThreads * sizeof(pthread_t));

    tP->start = 0;
    tP->stop = 0;
    tP->currentTask = 0;
    tP->activeTasks = 0;

    pthread_mutex_init(&tP->lock, NULL);
    pthread_cond_init(&tP->cond, NULL);
    pthread_cond_init(&tP->done, NULL);

    for (int i = 0; i < tP->nThreads; i++) {
        pthread_create(&tP->tPool[i], NULL, worker, tP);
    }
}


int main(void){
    double** matA = createMatrix(100, 2000);
    double** matB = createMatrix(80000, 2000);
    double** res  = createzMatrix(100, 80000);

    tile* tilesA = createTiles(100, 2000);
    tile* tilesB = createTiles(80000, 2000);

    taskQueue* tQ = createTaskQueue(tilesA, tilesB);
    createTaskQueue(tilesA, tilesB);
    printf("\n%d\n", tQ->nTasks);

    threadPool* tP = malloc(sizeof(threadPool));
    tP->matA = matA;
    tP->matB = matB;
    tP->resMat = res;
    tP->tQ = tQ;
    tP->currentTask = 1;
    tP->start = 0;
    tP->stop = 0;
    createThreadPool(tP);
    pthread_mutex_lock(&tP->lock);
    tP->activeTasks = tP->tQ->nTasks;
    tP->start = 1;
    pthread_cond_broadcast(&tP->cond);
    pthread_mutex_unlock(&tP->lock);

    pthread_mutex_lock(&tP->lock);
while (tP->start) {
    pthread_cond_wait(&tP->done, &tP->lock);
}
pthread_mutex_unlock(&tP->lock);


    printf("First row (first 10 values):\n");
    for (int j = 0; j < 10; j++) {
        printf(" %f ", res[0][j]);
    }
    printf("\n");
        pthread_mutex_lock(&tP->lock);
    tP->stop = 1;
    pthread_cond_broadcast(&tP->cond);
        pthread_mutex_unlock(&tP->lock);
    return 0;
}
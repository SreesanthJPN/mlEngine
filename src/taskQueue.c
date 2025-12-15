#include<stdio.h>
#include<malloc.h>
#include "2dTiles.h"

typedef struct taskArgs{
    tileData tileA;
    tileData tileB;
}taskArgs;

typedef struct taskQueue{
     double** matA;
     double** matB;
     taskArgs* tks;
     int nTasks;
}taskQueue;

taskQueue* createTaskQueue(tile* matATiles, tile* matBTiles, double** matA, double** matB){
    taskQueue* tQ = (taskQueue*)malloc(sizeof(taskQueue));
    tQ->nTasks = matATiles->nTiles * matBTiles->nTiles;
    tQ->tks = (taskArgs*)malloc(tQ->nTasks * sizeof(taskArgs));
    int c = 0;
    for(int i = 0; i < matBTiles->nTiles; i++){
        for(int j = 0; j < matATiles->nTiles; j++){
            taskArgs tArg;
            tArg.tileA = matATiles->tD[j];
            tArg.tileB = matBTiles->tD[i];
            tQ->tks[c] = tArg;
            c++;
        }
    }
    return tQ;
}
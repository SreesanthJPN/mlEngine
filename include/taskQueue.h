#ifndef TASKQUEUE_H
#define TASKQUEUE_H

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

taskQueue* createTaskQueue(tile* matATiles, tile* matBTiles, double** matA, double** matB);

#endif
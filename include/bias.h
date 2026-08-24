#ifndef BIAS
#define BIAS

#ifdef __cplusplus
extern "C" {
#endif

#include "threadss.h"

typedef struct biasPool{
    pthread_t* biasP;
    int start, stop, nThreads, currentRows;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    double** weight;
    int matRows, matCols;
    double** bias;
}biasPool;

void* biasWorker(void* args);

biasPool* createBiasPool();

void broadcastBias(biasPool* bP, double** mat, double** bias, int rows, int cols);


void deleteBiasPool(biasPool* bP);

#ifdef __cplusplus
}
#endif

#endif 
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
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

void* biasWorker(void* args){
    biasPool* bP = (biasPool*)args;

    while(1){
        pthread_mutex_lock(&bP->lock);
        while(!bP->start && !bP->stop){
         pthread_cond_wait(&bP->cond, &bP->lock);   
        }
        if(bP->stop){
            pthread_mutex_unlock(&bP->lock);
            return NULL;
        }
        if(bP->currentRows >= bP->matRows){
            bP->start = 0;
            pthread_cond_signal(&bP->cond);
            pthread_mutex_unlock(&bP->lock);
            continue;
        }
        int currRow = bP->currentRows++;
        pthread_mutex_unlock(&bP->lock);
        for(int i = 0; i < bP->matCols; i++){
            bP->weight[currRow][i] += bP->bias[i][0];
        }
    }
}

biasPool* createBiasPool(){
    biasPool* bP = (biasPool*)malloc(sizeof(biasPool));
    bP->nThreads = sysconf(_SC_NPROCESSORS_ONLN);
    bP->biasP = (pthread_t*)malloc(bP->nThreads*sizeof(pthread_t));
    pthread_mutex_init(&bP->lock, NULL);
    pthread_cond_init(&bP->cond, NULL);
    bP->start = 0;
    bP->stop = 0;
    bP->currentRows = 0;
    for(int i = 0; i < bP->nThreads; i++){
        pthread_create(&bP->biasP[i], NULL, biasWorker, (void*)bP);
    }
    return bP;
}


void broadcastBias(biasPool* bP, double** mat, double** bias, int rows, int cols){
    pthread_mutex_lock(&bP->lock);
    bP->weight = mat;
    bP->bias = bias;
    bP->matRows = rows;
    bP->matCols = cols;
    bP->currentRows = 0;
    bP->stop = 0;
    bP->start = 1;
    pthread_cond_broadcast(&bP->cond);
    while(bP->start){
        pthread_cond_wait(&bP->cond, &bP->lock);
    }
    pthread_mutex_unlock(&bP->lock);
}


void deleteBiasPool(biasPool* bP){
    pthread_mutex_lock(&bP->lock);
    bP->stop = 1;
    pthread_cond_broadcast(&bP->cond);
    pthread_mutex_unlock(&bP->lock);
    for(int i = 0; i < bP->nThreads; i++){
        pthread_join(bP->biasP[i], NULL);
    }
    pthread_mutex_destroy(&bP->lock);
    pthread_cond_destroy(&bP->cond);
    free(bP->biasP);
    free(bP);
}
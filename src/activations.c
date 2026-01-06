#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include<math.h>
#include "threadss.h"
#include<stdio.h>

double ReLu(double x){
    if(x > 0.0f) return x;
    else return 0.0f;
}

int Sigmoid(double x){
    return 1/1 + (exp(-x));
}

double leakyReLu(double x){
    if(x > 0.0f) return x;
    else return x * 0.01f;
}

double Tanh(double x){
    return (exp(x) - exp(-x))/(exp(x) + exp(-x));
}

typedef enum {
    relu,
    leakyRelu,
    sigmoid,
    tanH
} activation;


typedef struct activationThreadPool{
    pthread_t* pool;
    double** mat;
    int rows, cols;
    activation activ;
    int start, stop, nThreads, availableTask;
    pthread_mutex_t lock;
    pthread_cond_t cond;
}activationThreadPool;


void* activationWorker(void *args){
    activationThreadPool* aTP = (activationThreadPool*)args;

    while(1){
        pthread_mutex_lock(&aTP->lock);
        while(!aTP->start && !aTP->stop){
            pthread_cond_wait(&aTP->cond, &aTP->lock);
        }
        if(aTP->stop){
            pthread_mutex_unlock(&aTP->lock);
            return NULL;
        }
        if(aTP->availableTask >= aTP->rows){
            aTP->start = 0;
            pthread_cond_signal(&aTP->cond);
            pthread_mutex_unlock(&aTP->lock);
            continue;
        }
        int currentRow = aTP->availableTask++;
        pthread_mutex_unlock(&aTP->lock);

        for(int i = 0; i < aTP->cols; i++){
            switch (aTP->activ) {
    case relu:
        aTP->mat[currentRow][i] = ReLu(aTP->mat[currentRow][i]);
        break;
    case leakyRelu:
        aTP->mat[currentRow][i] = leakyReLu(aTP->mat[currentRow][i]);
        break;
    case sigmoid:
        aTP->mat[currentRow][i] = Sigmoid(aTP->mat[currentRow][i]);
        break;
    case tanH:
        aTP->mat[currentRow][i] = Tanh(aTP->mat[currentRow][i]);
        break;
}
    }
    }
}

activationThreadPool* createActivationThreadPool(){
    activationThreadPool* aTP = (activationThreadPool*)malloc(sizeof(activationThreadPool));
    aTP->nThreads = sysconf(_SC_NPROCESSORS_ONLN);
    aTP->pool = (pthread_t*)malloc(aTP->nThreads*sizeof(pthread_t));
    pthread_mutex_init(&aTP->lock, NULL);
    pthread_cond_init(&aTP->cond, NULL);
    aTP->start = 0;
    aTP->stop = 0;
    aTP->availableTask = 0;
    for(int i = 0; i < aTP->nThreads; i++){
        pthread_create(&aTP->pool[i], NULL, activationWorker, (void*)aTP);   
    }
    return aTP;
}

void startActivation(activationThreadPool* aTP, double** mat, int rows, int cols, activation activationType){
    pthread_mutex_lock(&aTP->lock);
    aTP->mat = mat;
    aTP->rows = rows;
    aTP->cols = cols;
    aTP->availableTask = 0;
    aTP->start = 1;
    aTP->stop = 0;
    aTP->activ = activationType;
    pthread_cond_broadcast(&aTP->cond);
    while(aTP->start){
        pthread_cond_wait(&aTP->cond, &aTP->lock);
    }
    pthread_mutex_unlock(&aTP->lock);
}

void deleteActPool(activationThreadPool* aTP){
    aTP->stop = 1;
    free(aTP->pool);
    free(aTP);
}
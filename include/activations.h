#ifndef ACTIVATIONS
#define ACTIVATIONS

#ifdef __cplusplus
extern "C" {
#endif

#include "threadss.h"

double ReLu(double x);

double Sigmoid(double x);

double leakyReLu(double x);

double Tanh(double x);

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


void* activationWorker(void *args);

activationThreadPool* createActivationThreadPool();

void startActivation(activationThreadPool* aTP, double** mat, int rows, int cols, activation activationType);

void deleteActPool(activationThreadPool* aTP);

#ifdef __cplusplus
}
#endif

#endif 
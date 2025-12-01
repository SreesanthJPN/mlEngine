#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>

#include "threadPool.h"

float randf(){ return 0.01*((float)rand()/(float)RAND_MAX); }


typedef struct matrix{
    int rows, cols;
    float** matData;
}matrix;

float** createMatrix(matrix* mat){

    int rows = mat->rows;
    int cols = mat->cols;

    float** twoDarray = (float**)malloc(rows * sizeof(float*));

    if(twoDarray == NULL){
        perror("Failed to allocate memory for the matrix");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < rows; i++){

        twoDarray[i] = malloc(cols * sizeof(float*));

        if(twoDarray[i] == NULL){
            perror("Failed to allocate memory");

            for(int j = 0; j < i; j++){
                free(twoDarray[j]);
            }
            free(twoDarray);
            exit(EXIT_FAILURE);
        }
        for(int j = 0; j < cols; j++){
            twoDarray[i][j] = randf();
        }
    }
    return twoDarray;
}

long maxThreads(long percent){
    return sysconf(_SC_NPROCESSORS_ONLN) * (percent/100);
}

matrix* transposeMat(matrix* mat){

    matrix *transMat;
    transMat = (matrix *)malloc(sizeof(matrix));

    transMat->rows = mat->cols;
    transMat->cols = mat->rows;

    transMat->matData = createMatrix(transMat);

    for(int i = 0; i < mat->rows; i++){
        for(int j = 0; j < mat->cols; j++){
            transMat->matData[i][j] = mat->matData[j][i];
        }
    }
    return transMat;
}

float getValueAtidx(float* row, float* col){
    if(sizeof(row) != sizeof(col)){
        perror("Dimention mismatch");
        exit(EXIT_FAILURE);
    }
    
}


void freeArray(matrix* mat){
    float** data = mat->matData;
    for(int i = 0;i < mat->rows; i++){
        free(data[i]);
    }
    free(data);
    free(mat);
}



matrix* mathMul(matrix* m1, matrix* m2, threadPool* pool){

    if (m1->cols != m2->rows) {
        perror("Invalid dimensions for matrix multiplication");
        exit(EXIT_FAILURE);
    }

    matrix* result = malloc(sizeof(matrix));
    result->rows = m1->rows;
    result->cols = m2->cols;
    result->matData = createMatrix(result); 

    matrix* m2T = transposeMat(m2);
    taskQueue* tQueue = createTaskQueue(m1->matData, m1->rows, m1->cols, m2T->matData, m2T->rows);

    pthread_mutex_lock(&pool->taskLock);

    pool->tasks = tQueue;
    pool->resultMat = result->matData;
    pool->nextTask = 0;
    pool->taskReady = 1;

    pthread_cond_broadcast(&pool->taskCond);

    pthread_mutex_unlock(&pool->taskLock);

    while (1) {
        pthread_mutex_lock(&pool->taskLock);
        int done = (pool->nextTask >= pool->tasks->taskCount);
        pthread_mutex_unlock(&pool->taskLock);

        if (done) break;
        usleep(1000);
    }
    return result;
}
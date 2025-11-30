#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<memory.h>

void* task(float* row1, int cols, float** mat2, int rows){
    float *result = (float*)malloc(sizeof(mat2)/sizeof(mat2[0]) * sizeof(float));
    for(int i = 0; i < rows; i++){
        float temp = (float)0;
        for(int j = 0; j < cols; j++){
            temp += row1[j] * mat2[i][j];
        }
    result[i] = temp;
    }
    return (void*)result;
}

typedef struct funcArgs{
    float* row1;
    int rows;
    float** mat2;
    int cols;
    int resultRow;
}funcArgs;

funcArgs createArgs(float* r1, int r, float** m2, int c, int resultRow) {
    funcArgs a;
    a.row1 = r1;
    a.rows = r;
    a.mat2 = m2;
    a.cols = c;
    a.resultRow = resultRow;
    return a;
}


typedef struct taskQueue{
    funcArgs* tQueue;
    float** mat1;
    int cols;
    int mat1Rows;
    float** mat2;
    int mat2Rows;
    int taskCount;
}taskQueue;

taskQueue* createTaskQueue(float** mat1, int m1Rows, int cols, float** mat2, int m2Rows){
    taskQueue* tempQueue = (taskQueue*)malloc(sizeof(taskQueue));
    tempQueue->mat1 = mat1;
    tempQueue->mat2 = mat2;
    tempQueue->mat1Rows = m1Rows;
    tempQueue->mat2Rows = m2Rows;
    tempQueue->taskCount = m2Rows;

    tempQueue->tQueue = (funcArgs*)malloc(tempQueue->taskCount * sizeof(funcArgs));

    int c = 0;
    for(int i = 0; i < m1Rows; i++){
            funcArgs a = createArgs(mat1[i], m2Rows, mat2, cols, i);  
            tempQueue->tQueue[c] = a;   
            c++;  
    }
    return tempQueue;
}

typedef struct threadPool{
    long nThreads;
    pthread_t *tPool;
    taskQueue* tasks;
    int nextTask;
}threadPool;
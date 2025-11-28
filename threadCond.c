#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

void* task(float* row1, float** mat2, int size){
    float *result = (float*)malloc(sizeof(row1)/sizeof(row1[0]) * sizeof(float));
    for(int i = 0; i < sizeof(mat2)/sizeof(row1); i++){
        float temp = (float)0;
        for(int j = 0; i < sizeof(row1)/sizeof(row1[0]); j++){
            temp += row1[j] + mat2[i][j];
        }
    result[i] = temp;
    }
    return (void*)result;
}

typedef float (*task)(float*, float*);

typedef struct{
    task* Queue;
}taskQueue;

typedef struct{

}threadPool;
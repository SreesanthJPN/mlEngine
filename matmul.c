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

float** createMatrix(matrix* mat) {
    int rows = mat->rows;
    int cols = mat->cols;

    float** arr = malloc(rows * sizeof(float*));

    for (int i = 0; i < rows; i++) {
        arr[i] = malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++)
            arr[i][j] = 0.0f;
    }
    return arr;
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
            transMat->matData[j][i] = mat->matData[i][j];
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


matrix* mathMul(matrix* m1, matrix* m2, threadPool* pool) {
    matrix* result = malloc(sizeof(matrix));
    result->rows = m1->rows;
    result->cols = m2->cols;
    result->matData = createMatrix(result);

    matrix* m2T = transposeMat(m2);

    pool->A = m1->matData;
    pool->rowsA = m1->rows;
    pool->colsA = m1->cols;
    pool->resultMat = result->matData;

    pool->tiles = tileMatrix(m2T->matData, m2T->rows, m2T->cols,
                             pool->tileSize, &pool->tileCount);

    pthread_mutex_lock(&pool->taskLock);
    pool->taskReady = 1;
    pthread_cond_broadcast(&pool->taskCond);
    pthread_mutex_unlock(&pool->taskLock);

    pthread_barrier_wait(&pool->barrier);

    return result;
}

int main(){

    threadPool* pool = malloc(sizeof(threadPool));
createThreadPool(pool, 128);

    float** m = (float**)malloc(2*sizeof(float*));

    for(int i = 0; i < 2; i++){
        float* temp = (float*)malloc(2*sizeof(float));
        for(int j = 0; j < 2; j++){
            temp[j] = j+1;
        }
        m[i] = temp;
    }

        for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){
            printf("%f " , m[i][j]);
        }
        printf("\n");
    }

    matrix *m1 = malloc(sizeof(matrix));
    m1->rows = 2;
    m1->cols = 2;
    m1->matData = m;

    matrix *m2 = malloc(sizeof(matrix));
    m2->rows = 2;
    m2->cols = 2;
    m2->matData = m;

    clock_t start = clock();
    matrix *m3 = mathMul(m1, m2, pool);
    clock_t end = clock();
    printf("Time taken -> %f \n", ((double)end - start)/CLOCKS_PER_SEC);
            for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){
            printf("%f " , m3->matData[i][j]);
        }
        printf("\n");
    }
}
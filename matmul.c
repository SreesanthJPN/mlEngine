#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>

float randf(){ return (float)rand()/(float)RAND_MAX; }

struct matrix{
    int rows, cols;
    float** matData;
};

float getValueAtidx(float* row, float* col){
    if(sizeof(row) != sizeof(col)){
        perror("Dimention mismatch");
        exit(EXIT_FAILURE);
    }
    
}

struct matrix* mathMul(struct matrix* m1, struct matrix* m2){
    
    if(m1->cols != m2->rows){
        perror("Invalid dimentions for matrix multiplication");
        exit(EXIT_FAILURE);
    }
    
    float** matrix1 = m1->matData;
    float** matrix2 = m2->matData;

    struct matrix* result = malloc(sizeof(struct matrix));

    result->rows = m1->rows;
    result->cols = m2->cols;
    result->matData = createMatrix(result, result->rows, result->cols);

    int noOfoperations = result->rows * result->cols;


}

void freeArray(struct matrix* mat){
    float** data = mat->matData;
    for(int i = 0;i < mat->rows; i++){
        free(data[i]);
    }
    free(data);
    free(mat);
}

float** createMatrix(struct matrix* mat, int rows, int cols){

    float** twoDarray = (float**)malloc(rows * sizeof(float));

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
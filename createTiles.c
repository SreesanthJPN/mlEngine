#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#define MAX_SIZE 1000000;

typedef struct tile{
    float*** tiles;
    int nTiles;
    int lastRows;
}tile;

tile* getTiles(float** mat, int rows, int cols, int nRowsPerTile){
    int finalRows = rows%nRowsPerTile;
    tile* t = (tile*)malloc(sizeof(tile));
    t->lastRows = finalRows;
    if(rows < nRowsPerTile){
        t->tiles = (float***)malloc(1*sizeof(float**));
        t->tiles[0] = mat;
        return t;
    }

    t->tiles = (float***)malloc((((rows - finalRows)/nRowsPerTile) + finalRows)*sizeof(float**));
    t->nTiles = ((rows - finalRows)/nRowsPerTile) + finalRows;
    for(int i = 0;i < (rows - finalRows)/nRowsPerTile; i++){
        float** temp = (float**)malloc(nRowsPerTile * sizeof(float*));
        for(int j = i*nRowsPerTile; j < i*nRowsPerTile + nRowsPerTile; j++){
            float* tempRow = (float*)malloc(cols*sizeof(float));
            tempRow = mat[j];
            temp[j] = mat[j];
        }
        t->tiles[i] = temp;
    }
    float** temp = (float**)malloc(finalRows * sizeof(float*));
    int i = 0;
    for(; i < finalRows; i++){
        temp[i] = mat[rows - finalRows + i];
    }
    t->tiles[(rows - finalRows)/nRowsPerTile + i] = temp;
    return t;
}

float** createMatrix(int rows, int cols) {
    srand(time(NULL));
    float** arr = malloc(rows * sizeof(float*));

    for (int i = 0; i < rows; i++) {
        arr[i] = malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++)
            arr[i][j] = 0.001*(double)rand() / RAND_MAX;
    }
    return arr;
}

int main() {
    float** m1 = createMatrix(14, 3);
    tile* t = getTiles(m1,14, 3, 3);
    printf("%d", t->nTiles);
}
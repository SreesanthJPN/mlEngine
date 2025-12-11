#include<stdio.h>
#include<memory.h>
#include<malloc.h>
#include <immintrin.h>

typedef struct tileData{
int rowStart, rowEnd, colStart, colEnd;
}tileData;

typedef struct tile{
    tileData* tD;
    int tiledRows, tiledCols, nTiles, __init_rows, __init_cols;
}tile;

tile* createTiles(double** mat, int rows, int cols){
    size_t size = (long int)rows*cols*8;
    tile *Tile = (tile*)malloc(sizeof(tile));
    if(size <= (long int)300000){
        Tile->nTiles = 1;
        Tile->tiledCols = cols;
        Tile->tiledRows = rows;
        Tile->__init_cols = cols;
        Tile->__init_rows = rows;
        Tile->tD = (tileData*)malloc(sizeof(tileData));
        Tile->tD->rowStart = 0;
        Tile->tD->rowEnd = rows - 1;
        Tile->tD->colStart = 0;
        Tile->tD->colEnd = cols - 1;
        return Tile;
    }
    else{
        int tRows = rows;
        int tCols = cols;
        while(size > (long int)300000){
            if(tCols/2 < (int)sizeof(__m256)){
                tRows /= 2;
            }
            else{
                tRows /= 2;
                tCols /= 2;
            }
            size = (long int)tRows * (long int)tCols *sizeof(double);
        }
        int i = 0;
        int j = 0;
        for(;i < rows - (tRows + (rows % tRows)); i+tRows){
            for(;j < cols - (tCols + (cols % tCols)); i+tCols){
                tileData* tempTileData = (tileData*)malloc(sizeof(tileData));
                tempTileData->rowStart = i;
                tempTileData->rowEnd = i + tRows;
                tempTileData->colStart = j;
                tempTileData->colEnd = j + tCols;
                
            }

        }
    }
}
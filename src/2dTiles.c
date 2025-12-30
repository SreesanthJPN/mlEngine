#include<stdio.h>
#include<malloc.h>
#include "findL3.h"

#define TILE_SIZE_LIMIT (1ULL * 1024 * 1024)

typedef struct tileData {
    int rowStart, rowEnd;   // [rowStart, rowEnd)
    int colStart, colEnd;   // [colStart, colEnd)
} tileData;

typedef struct tile {
    tileData* tD;
    int tiledRows;
    int tiledCols;
    int nTiles;
    int __init_rows;
    int __init_cols;
} tile;

static inline int min_int(int a, int b) {
    return (a < b) ? a : b;
}

tile* createTiles(int rows, int cols){

    if (rows <= 0 || cols <= 0) {
        return NULL;
    }

    tile* Tile = (tile*)malloc(sizeof(tile));
    if (!Tile) return NULL;

    Tile->__init_rows = rows;
    Tile->__init_cols = cols;

    size_t bytes_per_row = (size_t)cols * sizeof(double);
    size_t max_rows = getL3Cache()*0.3 / bytes_per_row;

    if (max_rows == 0) {
        /* Even one row does not fit */
        free(Tile);
        return NULL;
    }

    /* If entire matrix fits in one tile */
    if ((size_t)rows <= max_rows) {
        Tile->nTiles     = 1;
        Tile->tiledRows  = rows;
        Tile->tiledCols  = cols;

        Tile->tD = (tileData*)malloc(sizeof(tileData));
        if (!Tile->tD) {
            free(Tile);
            return NULL;
        }

        Tile->tD[0].rowStart = 0;
        Tile->tD[0].rowEnd   = rows;
        Tile->tD[0].colStart = 0;
        Tile->tD[0].colEnd   = cols;

        return Tile;
    }

    /* Row tiling */
    int row_tile = (int)max_rows;
    int num_row_tiles = (rows + row_tile - 1) / row_tile;

    Tile->tiledRows = row_tile;
    Tile->tiledCols = cols;
    Tile->nTiles    = num_row_tiles;

    Tile->tD = (tileData*)malloc((size_t)num_row_tiles * sizeof(tileData));
    if (!Tile->tD) {
        free(Tile);
        return NULL;
    }

    for (int t = 0; t < num_row_tiles; ++t) {
        int r0 = t * row_tile;
        int r1 = min_int(r0 + row_tile, rows);

        Tile->tD[t].rowStart = r0;
        Tile->tD[t].rowEnd   = r1;
        Tile->tD[t].colStart = 0;
        Tile->tD[t].colEnd   = cols;
    }

    return Tile;
}


tile* tileByRows(int rows, int cols, int tCols){
    size_t size = (size_t)rows*(size_t)cols*sizeof(double);
    tile *Tile = (tile*)malloc(sizeof(tile));
    int tRows;
        Tile->__init_cols = cols;
        Tile->__init_rows = rows;
    if(size <= getL3Cache()*0.3){
        Tile->nTiles = 1;
        Tile->tiledCols = cols;
        Tile->tiledRows = rows;
        Tile->tD = (tileData*)malloc(sizeof(tileData));
        Tile->tD->rowStart = 0;
        Tile->tD->rowEnd = rows - 1;
        Tile->tD->colStart = 0;
        Tile->tD->colEnd = cols - 1;
        return Tile;
    }
    else{
        int tRows = rows;
        while(size > getL3Cache()*0.3){
            tRows /= 2;
            size = (long int)tRows * (long int)tCols *sizeof(double);
        }
        int i = 0;
        int j;
        Tile->tiledRows = tRows;
        Tile->tiledCols = tCols;
        Tile->nTiles = (rows / tRows) * (cols / tCols);
        Tile->tD =  (tileData*)malloc((rows / tRows) * (cols / tCols) * sizeof(tileData));
        int c = 0;
        for(;i < rows - (tRows + (rows % tRows)); i = i+tRows){
            j=0;
            for(;j < cols - (tCols + (cols % tCols)); j = j+tCols){

                tileData tempTileData;
                tempTileData.rowStart = i;
                tempTileData.rowEnd = i + tRows;
                tempTileData.colStart = j;
                tempTileData.colEnd = j + tCols;
                Tile->tD[c] = tempTileData;
                c += 1;
                   
            }
                tileData tempTileData;
                tempTileData.rowStart = i;
                tempTileData.rowEnd = i + tRows;
                tempTileData.colStart = j;
                tempTileData.colEnd = j + tCols + (cols % tCols);
                Tile->tD[c] = tempTileData;
                c++;
        }
        for(;i < rows; i = i + (tRows + (rows % tRows))){
            j=0;
                for(;j < cols - (tCols + (cols % tCols)); j = j+tCols){

                tileData tempTileData;
                tempTileData.rowStart = i; 
                tempTileData.rowEnd = i+(tRows + (rows % tRows));
                tempTileData.colStart = j;
                tempTileData.colEnd = j + tCols;
                Tile->tD[c] = tempTileData;
                c++;
                  
            }
                tileData tempTileData;
                tempTileData.rowStart = i;
                tempTileData.rowEnd =  i + (tRows + (rows % tRows));
                tempTileData.colStart = j;
                tempTileData.colEnd = j + tCols + (cols % tCols);
                Tile->tD[c] = tempTileData;
                c++;
                    
    }
}
return Tile;
}
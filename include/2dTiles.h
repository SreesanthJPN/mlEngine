#ifndef _2DTILES_H
#define _2DTILES_H

typedef struct tileData{
int rowStart, rowEnd, colStart, colEnd;
}tileData;

typedef struct tile{
    tileData* tD;
    int tiledRows, tiledCols, nTiles, __init_rows, __init_cols;
}tile;

tile* createTiles(int rows, int cols);

tile* tileByRows(int rows, int cols, int tCols);

#endif
#ifndef _2DTILES_H
#define _2DTILES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tileData{
int rowStart, rowEnd, colStart, colEnd;
}tileData;

typedef struct tile{
    tileData* tD;
    int tiledRows, tiledCols, nTiles, __init_rows, __init_cols;
}tile;

tile* createTiles(int rows, int cols);

tile* tileByRows(int rows, int cols, int tCols);


#ifdef __cplusplus
}
#endif

#endif 
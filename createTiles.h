typedef struct tile{
    float*** tiles;
    int nTiles;
    int lastRows;
}tile;

tile* getTiles(float** mat, int rows, int cols, int nRowsPerTile);
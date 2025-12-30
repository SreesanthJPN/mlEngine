// #include<stdio.h>
// #include<malloc.h>
// #include "2dTiles.h"

// typedef struct taskArgs{
//     tileData tileA;
//     tileData tileB;
// }taskArgs;

// typedef struct taskQueue{
//      taskArgs* tks;
//      int nTasks;
// }taskQueue;

// taskQueue* createTaskQueue(tile* matATiles, tile* matBTiles){
//     taskQueue* tQ = (taskQueue*)malloc(sizeof(taskQueue));

// tQ->nTasks = 0;
//     int c = 1;
//     for(int i = 0; i < matBTiles->nTiles; i++){
//         for(int j = 0; j < matATiles->nTiles; j++){
//             taskArgs tArg;
//             if(matATiles->tD[j].colStart == matBTiles->tD[i].colStart && matATiles->tD[j].colEnd == matBTiles->tD[i].colEnd){
//             c += 1;
//         }    
//         }
//     }
//     tQ->nTasks = c;
    
//         tQ->tks = (taskArgs*)malloc(c* sizeof(taskArgs));
//         c = 0;
//         for(int i = 0; i < matBTiles->nTiles; i++){
//             for(int j = 0; j < matATiles->nTiles; j++){
//             taskArgs tArg;
//             if(matATiles->tD[j].colStart == matBTiles->tD[i].colStart && matATiles->tD[j].colEnd == matBTiles->tD[i].colEnd){
//             c += 1;
//             tArg.tileA = matATiles->tD[j];
//             tArg.tileB = matBTiles->tD[i];
//             tQ->tks[c] = tArg;
//         }  
//         }
//     }
//     return tQ;
// }

// int main(){
//     tile* tilesA = createTiles(1000, 200);
//     tile* tilesB = createTiles(7000 , 200);
//     taskQueue* tQ = createTaskQueue(tilesA, tilesB);
//     printf("%d", tQ->nTasks);
//     for(int i = 1; i < tQ->nTasks; i++){
//         printf("\n%d, %d, %d, %d\n",tQ->tks[i].tileB.rowStart, tQ->tks[i].tileB.rowEnd, tQ->tks[i].tileB.colStart, tQ->tks[i].tileB.colEnd);
//     }
// }
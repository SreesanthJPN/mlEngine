#include<pthread.h>
#include<stdio.h>

int count = 0;
pthread_mutex_t mut;        //init mutext object

void* testProcess(){
    for(int i = 0; i < 1000; i++){
        pthread_mutex_lock(&mut);       // lock the resource 
        count++;                        // perform operation
        pthread_mutex_unlock(&mut);     // unlock the resource
    }
}

int main(){

    pthread_t t1, t2;       // create threads
    pthread_mutex_init(&mut, NULL);     //init mutex

    pthread_create(&t1, NULL, testProcess, NULL);
    pthread_create(&t2, NULL, testProcess, NULL);    

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mut);

    printf("%d", count);
}
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void* funct(){
    printf("This is a parallel process");
    sleep(0.0003);
    printf("Process ended");
}

int main(){
    pthread_t t1, t2;
    pthread_create(&t1, NULL, &funct, NULL);
    pthread_create(&t2, NULL, &funct, NULL);
    pthread_join(t1,NULL);
    pthread_join(t2, NULL);
}
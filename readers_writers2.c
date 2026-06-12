#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t resource;
sem_t readTry;
pthread_mutex_t rmutex;
pthread_mutex_t wmutex;
sem_t serviceQueue;

int read_count=0; 
int write_count=0; 
int shared_data=100;

void* reader(void* arg){
    int id=*(int*)arg;
    while(1){
        sem_wait(&serviceQueue);
        sem_wait(&readTry);
        sem_post(&serviceQueue);

        pthread_mutex_lock(&rmutex);
        read_count++;
        if (read_count==1) sem_wait(&resource);
        pthread_mutex_unlock(&rmutex);

        sem_post(&readTry);

        printf("okuyucu %d okuyor: %d\n",id,shared_data);
        sleep(1);

        pthread_mutex_lock(&rmutex);
        read_count--;
        if(read_count==0) sem_post(&resource);
        pthread_mutex_unlock(&rmutex);

        sleep(2);
    }
}

void* writer(void* arg){
    int id=*(int*)arg;
    while(1){
        pthread_mutex_lock(&wmutex);
        write_count++;
        if(write_count==1) sem_wait(&readTry);
        pthread_mutex_unlock(&wmutex);

        sem_wait(&serviceQueue);
        sem_wait(&resource);
        sem_post(&serviceQueue);

        shared_data+=5;
        printf("--> yazıcı %d yazdı: %d\n",id,shared_data);
        sleep(2);

        sem_post(&resource);

        pthread_mutex_lock(&wmutex);
        write_count--;
        if(write_count==0) sem_post(&readTry);
        pthread_mutex_unlock(&wmutex);

        sleep(3);
    }
}

int main(){
    pthread_t r[3],w[2];
    int ids[3]={1,2,3};

    sem_init(&resource,0,1);
    sem_init(&readTry,0,1);
    sem_init(&serviceQueue,0,1);
    pthread_mutex_init(&rmutex,NULL);
    pthread_mutex_init(&wmutex,NULL);

    pthread_create(&r[0], NULL, reader, &ids[0]);
    pthread_create(&w[0], NULL, writer, &ids[0]);
    pthread_create(&r[1], NULL, reader, &ids[1]);
    pthread_create(&w[1], NULL, writer, &ids[1]);
    pthread_create(&r[2], NULL, reader, &ids[2]);

    for(int i=0; i<3; i++) pthread_join(r[i], NULL);
    for(int i=0; i<2; i++) pthread_join(w[i], NULL);

    return 0;
}
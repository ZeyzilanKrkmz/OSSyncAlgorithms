#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CHAIRS 3

sem_t customers;
sem_t barber;
pthread_mutex_t mutex;
int freeSeats=CHAIRS; 

void* BarberFunc(void* arg){
    while(1){
        sem_wait(&customers);
        pthread_mutex_lock(&mutex);

        freeSeats++;
        sem_post(&barber);
        pthread_mutex_unlock(&mutex);

        printf("berber: saç kesimi başladı...\n");
        sleep(2);
        printf("Berber: saç kesimi bitti.\n");




    }
}

void* CustomerFunc(void* arg){
    int id=*(int*)arg;
    pthread_mutex_lock(&mutex);
    if(freeSeats>0){
        freeSeats--;
        printf("müşteri %d: Sıraya oturdu. Boş Koltuk: %d\n",id,freeSeats);

        sem_post(&customers);
        pthread_mutex_unlock(&mutex);
        sem_wait(&barber);
        printf("müşteri %d: Dükkan dolu,tıraş olmadan ayrılıyor!\n",id);


    }else{
        pthread_mutex_unlock(&mutex);
        printf("müşteri %d : dükkan dolu, tıraş olmadan ayrılıyor!\n",id);
    }
    return NULL;
}
int main(){
    pthread_t b_tid;
    pthread_t c_tid[6];
    int c_ids[6];

    sem_init(&customers,0,0);
    sem_init(&barber,0,0);
    pthread_mutex_init(&mutex,NULL);

    pthread_create(&b_tid,NULL,BarberFunc,NULL);

    for(int i=0;i<6;i++){
        c_ids[i]=i+1;
        pthread_create(&c_tid[i],NULL,CustomerFunc,&c_ids[i]);
        sleep(rand()%2);
    }
    for (int i = 0; i < 6; i++) {
        pthread_join(c_tid[i], NULL);
    }
    
    // Berber ana döngüde sonsuza kadar çalıştığı için iptal ediyoruz
    pthread_cancel(b_tid); 
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5


int buffer[BUFFER_SIZE];//'n' arabellek boyutu 
int in=0;//üreticinin bir sonraki datayı nereye yazacağı,
int out=0;//tüketicinin bir sonraki datayı nereden okuyacağıdır.

sem_t empty;//boş slot sayısı
sem_t full;// dolu slot sayısı 
pthread_mutex_t mutex; //kritik bölge koruması

void *Producer(void* arg){
    int item;
    while(1){
        item=rand()%100;//rastgele bir veri üret
        sem_wait(&empty);//boş slot kontrolü, empty sayacını 1  azaltır. Eğer empty 0 ise üretici burada bloklanır.
        pthread_mutex_lock(&mutex);//kritik bölgeye gir

        buffer[in]=item;//veriyi arabelleğe yerleştir
        printf("üretici: %d değeri buffer[%d] konumuna yazıldı.\n",item,in);
        in=(in+1)%BUFFER_SIZE;//index +1
        pthread_mutex_unlock(&mutex);//kritik bölgeden çık
        sem_post(&full);//dolu slot sayısını arttır

        sleep(1);
    }
}

void* Consumer(void* arg){
    int item;
    while(1){
        sem_wait(&full);//içeride okuanacak data var mı
        pthread_mutex_lock(&mutex);//kritik bölge kilitlenir

        item=buffer[out];//veriyi çek
        printf("tüketici: buffer[%d] konumundan %d değeri okundu.\n",out,item);
        out=(out+1)%BUFFER_SIZE;//tüketici indexi bir sonraki konuma kaydırılır

        pthread_mutex_unlock(&mutex);//kritik bölge kilidi açılır.
        sem_post(&empty);

        sleep(2);
    }
}

int main(){
    pthread_t prod,cons;

    sem_init(&empty,0,BUFFER_SIZE);//başlangıçta empty semaforuna ilk değer olarak 5 verilir.
    sem_init(&full,0,0);//başlangıçta full semaforuna ilk değer 0 verilir.
    pthread_mutex_init(&mutex,NULL);//kilit mekanizması hazır hale getirilir.

    pthread_create(&prod,NULL,Producer,NULL);//prod ve cons fonsk.ları eşzamanlı çalışacak iki therad olarak başlatılır.
    pthread_create(&cons,NULL,Consumer,NULL);

    pthread_join(prod,NULL);//threadlerin bitmesini bekler.
    pthread_join(cons,NULL);

    sem_destroy(&empty);//semafor ve kaynaklar sisteme iade edilir.
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//aynı anda birden fazla okuyucu içeride olabilir
//aynı anda sadece bir yazıcı içeride olabilir
//içeride yazıcı varken okuyucu giremez, okuyucu varken yazıcı

pthread_mutex_t mutex;//readcnt güncellenirlen mutual excl. sağlar.
sem_t wrt;//paylaşılan kaynağa erişim
int readcnt=0;//içerideki okuyucu sayısı
int shared_data=0;//paylaşılan kritik kaynak

void* Reader(void* arg){
    int id=*(int*)arg;
    while(1){
        pthread_mutex_lock(&mutex);//okuyucu içeri girerken mutex'i kilitlesin
        readcnt++;// okuyucu +1
        if(readcnt==1){//gelen ilk okuyucuysa 
            sem_wait(&wrt);//kapıyı kilitle :)
        }
        pthread_mutex_unlock(&mutex);//diğer okuyuclar da girebilsin.

        printf("Okuyucu %d: ortak veriyi okudu: %d (içerideki okuyucu: %d)\n",id,shared_data,readcnt);
        sleep(1);

        pthread_mutex_lock(&mutex);//okuyucu azaltılacak diye kilitlenir
        readcnt--;
        if(readcnt==0){//çıkan, son okuyucuysa 
            sem_post(&wrt);//yazıcılar girebilsin
        }
        pthread_mutex_unlock(&mutex);
    }
}


void* Writer(void* arg){
    int id=*(int*)arg;
    while(1){
        sem_wait(&wrt);//kaynağı yazma için kilitle
        shared_data=rand()%100;
        printf("==> yazıcı %d: ortak veriyi güncelledi: %d\n",id,shared_data);
        sleep(2);
        sem_post(&wrt);//kaynağı serbest bırak
        sleep(3);
    }
}
int main(){
    pthread_t r[3], w[2];//3 okuyucu 2 yazıcı
    int ids[3]={1,2,3};
    pthread_mutex_init(&mutex,NULL);
    sem_init(&wrt,0,1);//wrt semaforuna bşlngç değeri 1 verilir

    pthread_create(&w[0],NULL,Writer,&ids[0]);
    pthread_create(&r[0],NULL,Reader,&ids[0]);
    pthread_create(&r[1],NULL,Reader,&ids[1]);
    pthread_create(&w[1],NULL,Writer,&ids[1]);
    pthread_create(&r[2],NULL,Reader,&ids[2]);

    for (int i=0;i<3;i++) pthread_join(r[i],NULL);
    for(int i=0;i<2;i++) pthread_join(w[i],NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;


//Eğer sisteme sürekli, ardı arkası kesilmeden yeni okuyucular gelirse; readcnt değeri hiçbir zaman 0'a düşmez.
//  readcnt sıfırlanmadığı için de sem_post(&wrt) çağrılmaz ve yazıcılar sonsuza kadar beklemek zorunda kalabilir.
//  Bu duruma işletim sistemlerinde Starvation (Açlık) adı verilir.
    

}
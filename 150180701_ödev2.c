// gerekli kütüphaneler
#define _GNU_SOURCE
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// semget() ve shmget() fonksiyonları için key ve id değeri dönmesine yardımcı olan tanımlar
#define KEYSEM ftok(strcat(get_current_dir_name(),argv[0]),1)
#define KEYSEM2 ftok(strcat(get_current_dir_name(),argv[0]),2)
#define KEYSEM3 ftok(strcat(get_current_dir_name(),argv[0]),3)
#define KEYSHM ftok(strcat(get_current_dir_name(),argv[0]),4)


//  semafor arttırma fonksiyonu
void sem_signal(int semid, int val){
  struct sembuf semaphore;
  semaphore.sem_num=0;
  semaphore.sem_op=val;
  semaphore.sem_flg=1;
  semop(semid,&semaphore,1);
}
// semafor azaltma fonksiyonu
void sem_wait(int semid, int val){
  struct sembuf semaphore;
  semaphore.sem_num=0;
  semaphore.sem_op=(-1*val);
  semaphore.sem_flg=1;
  semop(semid,&semaphore,1);
}
// signal-handling fonksiyonu
void mysignal(int signum){
  return ;
}
// sinyal numarası set etme fonksiyonu
void mysigset(int num){
  struct sigaction mysigaction;
  mysigaction.sa_handler=(void *)mysignal;
  mysigaction.sa_flags=0;
  sigaction(num,&mysigaction,NULL);
}

int main(int argc, char *argv[]){
  mysigset(12);   //sinyal numarası atanır
  int shmid=0;  // paylaşılan bellek idsi
  int *globalcp=NULL;   // paylaşılan bellek alanı
  int sem1=0,sem2=0,sem3=0; // semafor idleri
  int f,order=0;  // fork değeri tutmak ve hangi prosesin çalıştığını tutan değerler
  int child[3]; // 3 adet proses yaratılacağından 3 idlik yer açılır
  int i=1;
  // 3 tane çocuk proses yaratılır
  for (i=0;i<3;i++){
    f=fork();
    // sistemde hata olması durumunda
    if(f==-1){
        exit(1);
    }
    // çocuk prosesin başka bir çocuk yaratması önlenir
    if (f==0){
      break;
    }
    child[i]=f;
  }
  // anne prosesin yürütülmesi durumunu
  if (f!=0){
    // bütün kullanılacak semafor ve bellek alanı talebi semget shmget ile yapılır
    sem1=semget(KEYSEM,1,0700|IPC_CREAT);
    semctl(sem1,0,SETVAL,0);  // semafor değer=0
    sem2= semget(KEYSEM2,1,0700|IPC_CREAT);
    semctl(sem2,0,SETVAL,1);  // semafor değer=1
    sem3= semget(KEYSEM2,1,0700|IPC_CREAT);
    semctl(sem3,0,SETVAL,2);  // semafor değer =2
    shmid=shmget(KEYSHM,sizeof(int)*1001,0700|IPC_CREAT);
    // adres alanı bellek alanın idsi sayesinde bağlanır
    globalcp=(int *)shmat(shmid,0,0);
    // paylaşılan alanı bellekten ayırma
    shmdt(globalcp);
    sleep(2);
    for (i=0;i<3;i++){
      kill(child[i],12);  // bütün prosesler başlatılır
    }
    sem_wait(sem2,2);   // 2 çocuk prosesi bekler
    sem_wait(sem3,3);   // 3 çocuk prosesi bekler (hepsini)
    // bütün semafor ve paylaşılan alan iade edilir
    semctl(sem1,0,IPC_RMID,0);
    semctl(sem2,0,IPC_RMID,0);
    semctl(sem3,0,IPC_RMID,0);
    semctl(shmid,0,IPC_RMID,0);
    exit(0);
  }
  else {
    // hangi prosesin koştuğu order değeri ile görünür
    order=i;
    pause();
    //printf("%d.proses başlıyor ",order+1);
    // semafor ve paylaşılan bellek id değerleri döndürülür
    sem1=semget(KEYSEM,1,0);
    sem2=semget(KEYSEM2,1,0);
    sem3=semget(KEYSEM3,1,0);
    shmid=shmget(KEYSHM,sizeof(int)*1001,0);
    globalcp=(int *)shmat(shmid,0,0);
    // 1. prosesin çalışması başlar
    if(order==0){
      // 1..1000,0 değerleri paylaşılan alana yazılır
      for(i=0;i<1000;i++){
        globalcp[i]=i+1;
      }
      globalcp[1000]=0;
      sleep(1);
      // 1 saniye bekledikten sonra 2. prosese sinyal gönderir
      sem_signal(sem2,1);
    }
    // 2. prosesin çalışması başlar
    if(order==1){
      // 1. prosesin çalışmayı tamamlamasını bekler
      sem_wait(sem1,1);
      for(i=0;i<1001;i++){
        // 2'ye bölünen sayıları -1 yaparak alandaki sayıları değiştirir
        if(globalcp[i]%2==0){
          globalcp[i]=-1;
        }
      }
      sleep(1);
      // 1 saniye bekledikten sonra 3. prosese sinyal yollar
      sem_signal(sem3,1);
    }
    // 3. prosesin çalışması başlar
    if(order==2){
      // 2. prosesin çalışmasını bekler
      sem_wait(sem2,1);
      printf("Çıktı:\n");
      /* önceki prosesin yaptığı -1 ve 3'e bölünen değerleri atlayarak
          paylaşılan alandaki kurala uyan bütün sayıları yazdırır */
      for(i=0;i<1001;i++){
        if(globalcp[i]!=-1 && globalcp[i]%3!=0){
            printf("%d ",globalcp[i]);
        }
      }
      sleep(1);
    }
    // paylaşılan alanı her bir proses iade eder ve senkronizasyon sağlanır
    shmdt(globalcp);
    sem_signal(sem2,1);
    sem_signal(sem3,1);
    exit(0);
  }
  return 0;

}

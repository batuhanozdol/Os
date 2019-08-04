#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
  pid_t cocuk;
  pid_t cocuk2;
  int level=1;
  cocuk=fork();
  cocuk2=cocuk;
  // ilk doðan 2.seviyedeki çocuk proses
  if(cocuk==0){
    level++;
    cocuk=fork();
    cocuk2=cocuk;
    // 2. seviyedeki çocuðun yarattýðý 3. seviye prosesi
    if(cocuk==0){
      level++;
      cocuk=fork();
      // 3. seviyenin yarattýðý 4. seviye proses ve bilgileri
      if(cocuk==0){
        level++;
        printf("Seviyem -->%d\n",level);
        printf("Id degerim -->%d\n",getpid());
        if(cocuk){
          printf("Cocuklarimin id degeri --> %d %d\n\n",cocuk,cocuk2);
        }else {printf("\n");}
      }
      else {
        cocuk2=cocuk;
        cocuk=fork();
        // 3. seviyenin yarattýðý diðer 4. seviye prosesin bilgileri
        if(cocuk==0){
          level++;
          printf("Seviyem -->%d\n",level);
          printf("Id degerim -->%d\n",getpid());
          if(cocuk){
            printf("Cocuklarimin id degeri --> %d %d\n\n",cocuk,cocuk2);
          }else {printf("\n");}
        }
        // 3. seviye prosesin bilgileri
        else {
          printf("Seviyem -->%d\n",level);
          printf("Id degerim -->%d\n",getpid());
          if(cocuk){
            printf("Cocuklarimin id degeri --> %d %d\n\n",cocuk,cocuk2);
          }else {printf("\n");}
        }
      }
    }
    // 2. seviyenin yarattýðý diðer 3. seviyedeki proses
    else{
      cocuk=fork();
      // yaratýlan 3. seviye prosesin bilgileri
      if(cocuk==0){
        level++;
        printf("Seviyem -->%d\n",level);
        printf("Id degerim -->%d\n",getpid());
        if(cocuk){
          printf("Cocuklarimin id degeri --> %d %d\n\n",cocuk,cocuk2);
        }else {printf("\n");}
      }
      // 2. seviyedeki prosesin bilgileri
      else {
        printf("Seviyem -->%d\n",level);
        printf("Id degerim -->%d\n",getpid());
        if(cocuk){
          printf("Cocuklarimin id degeri --> %d %d\n\n",cocuk,cocuk2);
        }else {printf("\n");}
      }
    }
  }
  // ana proses için
  else {
    cocuk=fork();
    // ana proses 2.seviyede yeni çocuk proses yaratýrsa
    if(cocuk==0){
      level++;
      cocuk=fork();
      // 3. seviyede yeni çocuk proses doðarsa bilgileri yazdýrýlýr
      if(cocuk==0){
        level++;
        printf("Seviyem -->%d\n",level);
        printf("Id degerim -->%d\n",getpid());
        if(cocuk){
          printf("Cocuklarimin id degeri --> %d %d\n\n",cocuk,cocuk2);
        }else {printf("\n");}
      } 
      // 2. seviyedeki prosesin bilgileri
      else {
        printf("Seviyem -->%d\n",level);
        printf("Id degerim -->%d\n",getpid());
        if(cocuk){
          printf("Cocugumun id degeri --> %d\n\n",cocuk);
        }else {printf("\n");}
      }
    }
    // ana prosesin bilgileri
    else {
      printf("Seviyem -->%d\n",level);
      printf("Id degerim -->%d\n",getpid());
      if(cocuk){
        printf("Cocuklarimin id degeri --> %d %d\n\n",cocuk,cocuk2);
      }else {printf("\n");}
    }
  }
}

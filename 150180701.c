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
  // ilk do�an 2.seviyedeki �ocuk proses
  if(cocuk==0){
    level++;
    cocuk=fork();
    cocuk2=cocuk;
    // 2. seviyedeki �ocu�un yaratt��� 3. seviye prosesi
    if(cocuk==0){
      level++;
      cocuk=fork();
      // 3. seviyenin yaratt��� 4. seviye proses ve bilgileri
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
        // 3. seviyenin yaratt��� di�er 4. seviye prosesin bilgileri
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
    // 2. seviyenin yaratt��� di�er 3. seviyedeki proses
    else{
      cocuk=fork();
      // yarat�lan 3. seviye prosesin bilgileri
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
  // ana proses i�in
  else {
    cocuk=fork();
    // ana proses 2.seviyede yeni �ocuk proses yarat�rsa
    if(cocuk==0){
      level++;
      cocuk=fork();
      // 3. seviyede yeni �ocuk proses do�arsa bilgileri yazd�r�l�r
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

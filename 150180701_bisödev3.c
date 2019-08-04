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

// semget() ve shmget() fonksiyonlarý için key ve id deðeri dönmesine yardimci olan tanimlar
#define KEYSEM ftok(strcat(get_current_dir_name(),argv[0]),1)
#define KEYSEM2 ftok(strcat(get_current_dir_name(),argv[0]),2)
#define KEYSEM3 ftok(strcat(get_current_dir_name(),argv[0]),3)
#define KEYSHM ftok(strcat(get_current_dir_name(),argv[0]),4)

// rehberde tutulacak Insan bilgileri
typedef struct insan {
    char lastname[10];
    char firstname[10];
    int phone;
} Insan;

// aðaçta olan düðüm yapýsý
struct tree_node {
    Insan data;
    struct tree_node *left;
    struct tree_node *right;
};
// fonksiyon tanimlari
struct tree_node *insert(struct tree_node *p, Insan e);
struct tree_node *create_node (struct tree_node *q, struct tree_node *r, Insan e);
struct tree_node *delete_node (struct tree_node *p, char l[], char f[]);
int search_node(struct tree_node *p, char l[], char f[]);
void print_tree(struct tree_node *p);

//  semafor arttirma fonksiyonu
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
// sinyal numarasý set etme fonksiyonu
void mysigset(int num){
  struct sigaction mysigaction;
  mysigaction.sa_handler=(void *)mysignal;
  mysigaction.sa_flags=0;
  sigaction(num,&mysigaction,NULL);
}

int main(int argc, char *argv[]){
  mysigset(12);   //sinyal numarasi atanir
  Insan e;		
  int shmid=0;  // paylaþilan bellek idsi
  int *globalcp=NULL;   // paylaþýlan bellek alaný
  int ekle=0,ara=0,sil=0; //ekleme, arama ,silme semaforu idleri
  int f,order=0; 
  int child[3]; // 3 adet proses yaratilacagindan 3 idlik yer tutulur
  int i=1;
  struct tree_node *p = NULL; 	// ikili aðaç
  char ln[10];	// soyadý okumak için
  char fn[10];	// adý okumak için

	// En az iki ekleme iþlemi istendiðinden manuel olarak iki adet kiþi bilgisi aðaca eklenir
  printf("Ekle\nSoyad: ");
  scanf("%s", e.lastname);
  printf("Ad: ");
  scanf("%s", e.firstname);
  printf("Tel. no: ");
  scanf("%d", &e.phone);
  p = insert(p, e);
  printf("Ekleme tamamlandi\n\n");

  printf("Ekle\nSoyad: ");
  scanf("%s", e.lastname);
  printf("Ad: ");
  scanf("%s", e.firstname);
  printf("Tel. no: ");
  scanf("%d", &e.phone);
  p = insert(p, e);
  printf("Ekleme tamamlandi\n\n");
  
  // 3 tane çocuk proses yaratilir
  for (i=0;i<3;i++){
    f=fork();
    // sistemde hata olmasi durumunda
    if(f==-1){
        exit(1);
    }
    // çocuk prosesin baþka bir çocuk yaratmasi önlenir
    if (f==0){
      break;
    }
    child[i]=f;
  }
  // anne prosesin yürütülmesi durumunu
  if (f!=0){
    // bütün kullanilacak semafor ve bellek alani talebi semget shmget ile yapilir
    ekle=semget(KEYSEM,1,0700|IPC_CREAT);
    semctl(ekle,0,SETVAL,0);  // semafor degeri=0
    ara= semget(KEYSEM2,1,0700|IPC_CREAT);
    semctl(ara,0,SETVAL,1);  // semafor degeri=1
    sil= semget(KEYSEM2,1,0700|IPC_CREAT);
    semctl(sil,0,SETVAL,2);  // semafor degeri =2
    shmid=shmget(KEYSHM,sizeof(e)*6,0700|IPC_CREAT);
    // adres alani bellek alani idsi sayesinde baglanir
    globalcp=(int *)shmat(shmid,0,0);
    // paylasilan alani bellekten ayirma
    shmdt(globalcp);
    sleep(2);
    for (i=0;i<3;i++){
      kill(child[i],12);  // butun prosesler baslatilir
    }
    sem_wait(ara,2);	// semafor deðeri 2 azaltýlýr
    sem_wait(sil,3);   // semafor deðeri 3 azaltýlýr
    // butun semafor ve paylasilan alan iade edilir
    semctl(ekle,0,IPC_RMID,0);
    semctl(ara,0,IPC_RMID,0);
    semctl(sil,0,IPC_RMID,0);
    semctl(shmid,IPC_RMID,0);
    exit(0);
  }
  else {
    // hangi prosesin kostugu order degeri ile gorunur
    order=i;
    pause();
    // semafor ve paylasilan bellek id degerleri dondurulur
    ekle=semget(KEYSEM,1,0);
    ara=semget(KEYSEM2,1,0);
    sil=semget(KEYSEM3,1,0);
    shmid=shmget(KEYSHM,sizeof(e)*6,0);
    globalcp=(int *)shmat(shmid,0,0);
    
    // 1. çocuk olan ekleme prosesi kosturur
    if(order==0){
    	// silme prosesi beklenir
      sem_wait(sil,1);
      for(i=0;i<2;i++){
      	// iki adet kisi agaca eklenir
        printf("Ekle\nSoyad: ");
        scanf("%s", e.lastname);
        printf("Ad: ");
        scanf("%s", e.firstname);
        printf("Tel. no: ");
        scanf("%d", &e.phone);
        p = insert(p, e);
        printf("Ekleme tamamlandi\n\n");
      }
      sleep(1);
      // arama prosesine sinyal yollanýr
      sem_signal(ara,1);
    }
    // 2. proses olan arama prosesi kosturur
    if(order==1){
    	// hem ekleme hem silme prosesinden sinyal beklenir
      sem_wait(sil,1);
      sem_wait(ekle,1);
      for(i=0;i<2;i++){
      		// iki adet kiþinin arama sorgusu yapýlýr
          printf("Ara\nSoyad: ");
          scanf("%s", ln);
          printf("Ad: ");
          scanf("%s", fn);
          if (search_node(p,ln,fn)==1){
            printf("Kayit bulundu\n\n");
          }
          else {
            printf("Kayit bulunamadi\n\n");
          }
      }
      sleep(1);
    }
    // 3. çocuk olan silme prosesi kosturur
    if(order==2){
      // önceki proseslerden sinyal beklemeyceginden once calisir
      for(i=0;i<2;i++){
      	// iki adet silme islemi gerceklestirir
        printf("Sil\nSoyad: ");
        scanf("%s", ln);
        printf("Ad: ");
        scanf("%s", fn);
        // bulunan node silinir
        p = delete_node(p, ln, fn);
        printf("Silme tamamlandi\n\n");
      }
      sleep(1);
      // arama ve ekleme proseslerine sinyal gonderilir
      sem_signal(ara,1);
      sem_signal(ekle,1);
    }
    // paylasilan alani her bir proses iade eder ve senkronizasyon saglanir
    shmdt(globalcp);
    exit(0);
  }
  return 0;
}

int search_node(struct tree_node *p, char l[], char f[]) {
    // girilen soyadýn node'dan kýsa olmasý durumunda sol node'a gecilir
    if (strcmp(l, p->data.lastname) < 0) {
        search_node(p->left, l, f);
    }
    // soyad node'un soyad degerinden uzunsa sað node'a geçilir
    else if (strcmp(l, p->data.lastname) > 0) {
        search_node(p->right, l, f);
    }
    // soyadlar uyuþup adlarýn uyuþmamasý durumunda 
    else if (strcmp(l, p->data.lastname) == 0 && strcmp(f, p->data.firstname) != 0) {
        // ad node'un ad degerinden kýsaysa sol node'a geçilir
        if (strcmp(f, p->data.firstname) < 0) {
            search_node(p->left, l, f);
        }
        // ad node'un ad degerinden uzunsa sað node'a geçilir
        if (strcmp(f, p->data.firstname) > 0) {
            search_node(p->right, l, f);
        }
    }
    // girilen soyad ve ad bilgilerinin uyusmasi durumunda 
    else if (strcmp(l, p->data.lastname) == 0 && strcmp(f, p->data.firstname) == 0) {
        // kisinin bulunduguna dair 1 degeri dondurulur
        return 1;
      }
    // kisi bulunamazsa 0 degeri dondurulur
    else {
        return 0;
    }
}

// rekursif bir yapýda soyad ve ad anahtar deger uzunluguna bagli olarak kurulan ekleme islemi
struct tree_node *insert(struct tree_node *p, Insan e) {
  	// agacta root yoksa
 	if (p == NULL) {
     	// yeni node acilir
    	p = create_node(NULL, NULL, e);
 	}
 	// girilen soyad degeri parent node degerinden kýsaysa sola gecilir
 	else if (strcmp(e.lastname, p->data.lastname) < 0) { 
     	p->left = insert(p->left, e);
 	}
 	// girilen ad degeri parent node degerinden uzunsa saga gecilir
 	else if (strcmp(e.lastname, p->data.lastname) > 0) {
     	p->right = insert(p->right, e);
 	}
 	else {
    	// girilen ad degeri parent node degerinden kýsaysa sola gecilir
     	if (strcmp(e.firstname, p->data.firstname) < 0) {
        	p->left = insert(p->left, e);
    	}
    	// girilen ad degeri parent node degerinden uzunsa saga gecilir 
    	else if (strcmp(e.firstname, p->data.firstname) > 0) {
         	p->right = insert(p->right, e);
    	} 
    	else { // ayni kaydin bulunmasi durumunda
        	return p;
    	}
 	}
 	return p;
}

// agac Inorder olacagindan en soldaki node minimum olacaktir
struct tree_node *findmin(struct tree_node *p) {
    if (p->left != NULL) {
        findmin(p->left);
    }
    return p;
}

// yeni bir node olusturulur, girilen Insan degiskenindeki bilgiler bu node'a aktarilir
struct tree_node *create_node (struct tree_node *q, struct tree_node *r, Insan e) {
    struct tree_node* newnode;
    newnode = (struct tree_node*)(malloc(sizeof(struct tree_node)));
    newnode->data = e;
    newnode->left = q;
    newnode->right = r;
    return newnode;
}

struct tree_node *delete_node (struct tree_node *p, char l[], char f[]) {
  	// soyad degeri roottakinden kýsaysa sol node'a gidilir
    if (strcmp(l, p->data.lastname) < 0 || strcmp(f, p->data.firstname) != 0) {
      p->left = delete_node(p->left, l, f);
    }
 	// soyad degeri roottakinden uzunsa sag node'a gidilir
    else if (strcmp(l, p->data.lastname) > 0 || strcmp(f, p->data.firstname) != 0) {
      p->right = delete_node(p->right, l, f);
    }
 	// bulunulan node'da sag ve sol node'un olmamasý durumunda
    else if (p->left != NULL && p->right != NULL) {
      // sag minimum node root olarak ayarlanir
	  p->data = findmin(p->right)->data;
      p->right = delete_node(p->right, l, f);
   }
   	// solunda node yoksa 
    else if (p->left != NULL) {
      	p = p->left;
    }
 	// saginda node yoksa
    else if (p->right != NULL) {
      	p = p->right; 
    }
 	return p;
}

#include<stdio.h>
#include<semaphore.h>
#include<time.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<string.h>
#include<math.h>
#include<fcntl.h>
#include<errno.h>

#define BufferSize 20
#define ThreadNum 3

typedef int item;
typedef struct{
  int in;
  int out;
  item buff[BufferSize];
  }product;

sem_t *full1;
sem_t *empty1;
sem_t *mutex1;
product *ptr1;

float negexp_random(double lam)
{
  double a, r;
  do{
    a = (double)rand() / (double)RAND_MAX;
    
  }while ((a==0.0 || a ==1.0));
  r = (-1/lam)*log(a);
  return r;
  
}

void *consumer(void *b)
{
  float lambda = atof(b);
  float sleep_time = negexp_random(lambda);
  //printf("sleep %f\n", sleep_time);
  sleep(sleep_time);
  
  sem_wait(full1);
  sem_wait(mutex1);
  printf("PID is %d | TID is %lu | out_data is %d\n", getpid(), (unsigned long)pthread_self(), ptr1->buff[ptr1->out]);
  ptr1->buff[ptr1->out] = 0;
  ptr1->out = (ptr1->out +1) % BufferSize;
  sem_post(mutex1);
  sem_post(empty1);
  pthread_exit(0);
}


int main(int argc, char *argv[])
{
/*
  sem_unlink("full1");
  sem_unlink("empty1");
  sem_unlink("mutex1");
  shm_unlink("ShareBuffer1");
  */
  ///printf("aaa\n");
  //Initialize semaphore
  full1 = sem_open("full1", O_CREAT | O_RDWR, 0666, 0);
  empty1 = sem_open("empty1", O_CREAT | O_RDWR, 0666, BufferSize);
  mutex1 = sem_open("mutex1", O_CREAT | O_RDWR, 0666, 1);
  
  //share memory 
  int shm_fd = shm_open("ShareBuffer1", O_CREAT | O_RDWR, 0666);
  if (shm_fd<0){
    //fprintf(stderr, "shm_open error!!!\n");
    perror("shm_open failed!\n");
    exit(1);
    }
  //ftruncate(shm_fd, sizeof(product));
  ptr1 = (product*)mmap(0, sizeof(product), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
  if (ptr1 == MAP_FAILED){
    perror("mmap error!!!\n");
    exit(1);
    }
  //Initialize share memory
  //memset(ptr1, 0, sizeof(product));
 /* 
  int e,f,m;
  sem_getvalue(mutex1,&m);
  sem_getvalue(empty1,&e);
  sem_getvalue(full1,&f);
  printf("empty=%d, full=%d,mutex =%d\n",e,f,m);
  printf("in=%d, out=%d\n", ptr1->in, ptr1->out);
 
  int n;
  printf("BUffer:");
  for(n=0; n<BufferSize; n++){
    printf("%d ",ptr1->buff[n]);
  }
  printf("\n\n");
 */
  
  pthread_t tid[ThreadNum];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  int i;
  int loop;
  for(loop = 0; loop<10;loop++){
    for(i=0; i<ThreadNum; i++){pthread_create(&tid[i], &attr, consumer, argv[1]);}
    for(i=0; i<ThreadNum; i++){pthread_join(tid[i], NULL);}
  }
  
  //unmap memory
  //munmap(ptr, sizeof(product));
  shm_unlink("ShareBuffer1");
  //sem_unlink("full1");
  //sem_unlink("empty1");
  //sem_unlink("mutex1");
  
  return 0;
}
  
















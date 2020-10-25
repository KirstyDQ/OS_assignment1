#include<pthread.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>

#define phi_num 5
#define thinking 1
#define hungry 2
#define eating 3
#define test_times 5

int thread_id[phi_num];
pthread_t tid[phi_num];

pthread_cond_t cond_vars[phi_num];
pthread_mutex_t mutex;

int phi_state[phi_num];

void check_neighbor(int n)
{
  if ( (phi_state[(n+phi_num-1)%phi_num] != 3) && (phi_state[(n+1)%phi_num] != 3) && (phi_state[n] == 2) )
  {
    phi_state[n] = 3;
    printf("No. %d philosopher is eating!\n", n);
    pthread_cond_signal(&cond_vars[n]);
  }
}
 
  
void pickup_forks(int num)
{
  pthread_mutex_lock(&mutex);
  phi_state[num] = 2;
  
  //If I want to eat and left one and right one are not eating. 
  //Then I can eat
  check_neighbor(num);
  
  //while I am not eating,I should wait for my cond_var ready
  //when cond_var , I can get mutex
  while(phi_state[num] != 3)
    pthread_cond_wait(&cond_vars[num], &mutex);
  pthread_mutex_unlock(&mutex);
}

void return_forks(int num)
{
  pthread_mutex_lock(&mutex);
  //When I return the forks, I am thinking
  phi_state[num] = 1;
  printf("No. %d philosopher is thinking!\n", num);
  
  //check the neighbors' states. If neighbors are waiting to eat?
  check_neighbor((num+phi_num-1)%phi_num);
  check_neighbor((num+1)%phi_num);
  
  pthread_mutex_unlock(&mutex);
}
  
void *philosopher(void *a)
{  
  //get a random sleep time
  int num = *(int*)a;
  int sleep_time = (rand()%3)+1;
  
  //int i;
  //for(i=0 ; i<test_times;i++){
  while(1){
    sleep(sleep_time);
    pickup_forks(num);
    sleep(sleep_time);
    return_forks(num);
    }
}

int main(){
int i;

//initialize, 
//all philosophers are thinking
//philosopher's ID(number) is 0,1,2,3,4
// And initialize condition variable
for(i=0; i<phi_num; i++){
  phi_state[i]=1;
  thread_id[i]=i;
  pthread_cond_init(&cond_vars[i], NULL);
  }

//Initialize mutex lock
pthread_mutex_init(&mutex, NULL);

//create 5 thread for philosophers
for(i=0; i<phi_num; i++)
  pthread_create(&tid[i], 0, philosopher, (void*)&thread_id[i]);
for(i=0; i<phi_num; i++)
  pthread_join(tid[i], NULL);
  
return 0;

}

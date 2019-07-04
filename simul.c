/***********************************************************************/
/**      Author: Minas Spetsakis                                      **/
/**        Date: Jun. 2019                                         **/
/** Description: Skeleton code for Assgn. II                               **/
/***********************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "args.h"
#include "error.h"
/* added lib */
#include <string.h>
#include <stdlib.h>
#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>

#define THINKING 0
#define EATING 1
#define HUNGRY 2

/* default values for args */

int N = 5, T = 100;
double L = 0.1, M = 0.2;
int nblocked; /* The number of threads blocked */
int *chopstick_state;
pthread_mutex_t *chop_mutex;
pthread_mutex_t mutex_time_spend_on;
pthread_cond_t chair = PTHREAD_COND_INITIALIZER;

int condition = 0;
double time_spend_on[] = {0.0, 0.0, 0.0};
/*utility functions */
int total_chairs;

void check_chair()
{
  if (total_chairs > 0)
  {
    pthread_cond_signal(&chair);
  }
}

/***********************************************************************
                         P H I L O S O P H E R
************************************************************************/
void *philosopher(void *vptr)
{

 
  unsigned int seed; /* This is called from main without    */
  int pthrerr;       /* creating a new thread               */
  struct thread_arg *ptr;
  pid_t my_tid;
  ptr = (struct thread_arg *)vptr;
  my_tid = syscall(SYS_gettid);
  int nindex = my_tid % ptr->N;
 
  seed = ptr->seed;

  clock_t start, end;
  double time_used;

  while (1)
  {
    start = clock();
    pthread_mutex_lock(ptr->mutex);
 //   printf("Thread %d is blocked by condition variable start_line\n", nindex);
    nblocked++;
    if (nblocked == (ptr->N))
    {
      pthrerr = pthread_cond_signal(ptr->sclkblock);
      if (pthrerr != 0)
        fatalerr("Philosopher", 0, "Condition signal failed\n");
    }
      pthread_cond_wait(ptr->start_line, ptr->mutex); // blocks in the begining
      pthread_mutex_unlock(ptr->mutex);

    // lock chair

           pthread_mutex_lock(ptr->mutex);

          if (total_chairs == 0)
          {
            pthread_cond_wait(&chair,ptr->mutex);
            printf("chair all taken\n");            
          }
         total_chairs--;
         printf("philosopther %d take 1 chair, %d left\n",nindex,total_chairs);
         check_chair();
         pthread_mutex_unlock(ptr->mutex);


    if (ptr->state == THINKING)
    {
      printf("random number in thinking is %f\n",rand0_1(&seed));
      while (rand0_1(&seed) < ptr->lam)
        ;
      end = clock();
      //sleep(3);
      time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
      pthread_mutex_lock(&mutex_time_spend_on);
      time_spend_on[THINKING] += time_used;
      pthread_mutex_unlock(&mutex_time_spend_on);
      ptr->state = HUNGRY;
    }
    if (ptr->state == HUNGRY)
    {
      end = clock();
      time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
      pthread_mutex_lock(&mutex_time_spend_on);
      time_spend_on[HUNGRY] += time_used;
      pthread_mutex_unlock(&mutex_time_spend_on);
      ptr->state = EATING;
    }



    if (ptr->state == EATING)
    {
      // lock mutex for left chopstick
      pthread_mutex_lock(chop_mutex + nindex);
      // left chopstick avaiable
      if (chopstick_state[nindex] == 1)
      {
     //   printf("Philosopher %d: I got the left one!\n", nindex);
        chopstick_state[nindex] = 0;                            // lock left chopstick
        pthread_mutex_unlock(chop_mutex + nindex);              //unlock the left chopstick
        pthread_mutex_lock(chop_mutex + (nindex + 1) % ptr->N); // lock the right chopstick;

        // if the right chop is avaiable
        if (chopstick_state[(nindex + 1) % ptr->N] == 1)
        {
       
       //   printf("Philosopher %d: I got both!\n", nindex);
      //  printf("Philosopher %d: start eating!\n", nindex);
          chopstick_state[(nindex + 1) % ptr->N] = 0; // set to unavaibale
          pthread_mutex_unlock(chop_mutex + (nindex + 1) % ptr->N);
          // eat
          while (rand0_1(&seed) < ptr->mu) // start eating
            ;
          end = clock();
          time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
          pthread_mutex_lock(&mutex_time_spend_on);
          time_spend_on[EATING] += time_used;
          pthread_mutex_unlock(&mutex_time_spend_on);
          ptr->state = THINKING;

          /* lock the left & right chopsticks */
          pthread_mutex_lock(chop_mutex + nindex);
          pthread_mutex_lock(chop_mutex + ((nindex + 1) % ptr->N));
          pthread_mutex_lock(ptr->mutex);
          chopstick_state[nindex] = 1;                /*set the left chopstick available */
          chopstick_state[(nindex + 1) % ptr->N] = 1; /* set the right chopstick available */
           total_chairs++;
           check_chair();
            printf("Philosopher %d: finished eating,release chair! total chair is %d\n", nindex,total_chairs);
          /* unlock the left & right chopsticks */
          pthread_mutex_unlock(chop_mutex + nindex);
          pthread_mutex_unlock(chop_mutex + ((nindex + 1) % ptr->N));
          pthread_mutex_unlock(ptr->mutex);

         
        }
        else
        { /* the right is unavailable */
    //      printf("Philosopher %d: I cannot get the right one!\n\n", nindex);

          pthread_mutex_unlock(chop_mutex + ((nindex + 1) % ptr->N)); /* unlock the right chopstick */
          pthread_mutex_lock(chop_mutex + nindex);                    /* lock the left chopstick */
          chopstick_state[nindex] = 1;                                /* set the left chopstick available (put down the left one) */
          pthread_mutex_unlock(chop_mutex + nindex);                  /* unlock the left chopstick */
        //  sleep(1);                                                   /* wait for a while and try again later */
        }
      }
      else
      { /* the left chopstick is unavailable */

  //      printf("Philosopher %d: I cannot even get the left chopstick!\n\n",
  //             nindex);
        pthread_mutex_unlock(chop_mutex + nindex); /* unlock the left chopstick */

       // sleep(1); /* wait for a while and try again later */
      }
    }
  }

  return NULL;
}

/***********************************************************************
                                C L K
************************************************************************/
void *clk(void *vptr)
{
  int tick;
  int pthrerr;
  struct thread_arg *ptr;

  ptr = (struct thread_arg *)vptr;

  tick = ptr->T;
  printf("inside clock\n");
  //condition = 1;
  pthread_mutex_lock(ptr->mutex);
  printf("tick is %d\n",tick);
  for (int i = 0; i < tick; i++)
  {
    while (nblocked < ptr->N)
    {
      pthrerr = pthread_cond_wait(ptr->sclkblock, ptr->mutex);
      if (pthrerr != 0)
        fatalerr("Clock", 0, "Condition wait failed\n");
    }
    nblocked = 0;
    pthrerr = pthread_cond_broadcast(ptr->start_line);
    if (pthrerr != 0)
      fatalerr("Clock", 0, "Condition b/cast failed\n");
  }

  double average_thinking_time = time_spend_on[THINKING] / ptr->N;
  double average_eating_time = time_spend_on[EATING] / ptr->N;
  double average_hungry_time = time_spend_on[HUNGRY] / ptr->N;


  printf("Total thinking time: %f\n",time_spend_on[THINKING]);
  printf("Total hungry time %f\n",time_spend_on[HUNGRY]);
  printf("Total eating time %f\n",time_spend_on[EATING]);
  printf("\n");
  printf("\n");
  printf("Average thinking time: %f\n",average_thinking_time);
  printf("Average hungry time %f\n",average_eating_time);
  printf("Average eating time %f\n",average_hungry_time);


  exit(0);
}

/***********************************************************************
                               M A I N
************************************************************************/
int main(int argc, char **argv)
{

  int pthrerr, i;
  int nphil, nticks;
  double lam, mu;

  pthread_t tid;
  pthread_cond_t start_line,sclkblock; // all phil thread block on start_line condition var.
  pthread_mutex_t mutex;
  pthread_t *alltids;

  struct thread_arg *allargs;

  // initialize condition variables
  pthrerr = pthread_cond_init(&start_line, NULL);
  if (pthrerr != 0) /* Initializers never return an error code */
    fatalerr(argv[0], 0, "Initialization failed\n");
  // initialize condition variables
  pthrerr = pthread_cond_init(&sclkblock, NULL);
  if (pthrerr != 0) /* Initializers never return an error code */
    fatalerr(argv[0], 0, "Initialization failed\n");


  //initialize mutex
  pthrerr = pthread_mutex_init(&mutex, NULL);
  if (pthrerr != 0)
    fatalerr(argv[0], 0, "Initialization failed\n");

  //initialize mutex_time_increment
  pthrerr = pthread_mutex_init(&mutex_time_spend_on, NULL);
  if (pthrerr != 0)
    fatalerr(argv[0], 0, "Initialization failed\n");

  // assign global var
  nphil = N;
  nticks = T;
  lam = L;
  mu = M;
  nblocked = 0;

  // cmd_args start
  i = 1;
  while (i < argc - 1)
  {
    if (strncmp("-L", argv[i], strlen(argv[i])) == 0)
      lam = atof(argv[++i]);
    else if (strncmp("-M", argv[i], strlen(argv[i])) == 0)
      mu = atof(argv[++i]);
    else if (strncmp("-N", argv[i], strlen(argv[i])) == 0)
      nphil = atoi(argv[++i]);
    else if (strncmp("-T", argv[i], strlen(argv[i])) == 0)
      nticks = atoi(argv[++i]);
    else
      fatalerr(argv[i], 0, "Invalid argument\n");
    i++;
  }
  if (i != argc)
    fatalerr(argv[0], 0, "Odd number of args\n");
  // cmd arg done

  // allocate space for chopsticks

  chopstick_state = (int *)
      malloc((nphil) * sizeof(pthread_t));
  // memory allocating for thread args start
  allargs = (struct thread_arg *)
      malloc((nphil) * sizeof(struct thread_arg));

  if (allargs == NULL)
    fatalerr(argv[0], 0, "Out of memory\n");
  alltids = (pthread_t *)
      malloc((nphil) * sizeof(pthread_t));
  if (alltids == NULL)
    fatalerr(argv[0], 0, "Out of memory\n");
  // memory allocating for thread args done

  chop_mutex = (pthread_mutex_t *)malloc((nphil) * sizeof(pthread_mutex_t));
  if (chop_mutex == NULL)
    fatalerr(argv[0], 0, "Out of memory\n");
  //assigning initial values
  allargs[0].N = nphil;
  allargs[0].T = nticks;
  allargs[0].state = THINKING; // initial state
  allargs[0].lam = lam;
  allargs[0].mu = mu;
  allargs[0].start_line = &start_line;
  allargs[0].sclkblock = &sclkblock;
  allargs[0].mutex = &mutex;
  allargs[0].seed = 100; // random number

  total_chairs = nphil - 1;

  // create phil threads and initializing chopstick value
  int j;
  for (j = 0; j < nphil; j++)
  {

    chopstick_state[j] = 1; // initialize to avaiable
    allargs[j] = allargs[0];

    // initiliaze chop mutexes
    pthrerr = pthread_mutex_init(chop_mutex + j, NULL);
    if (pthrerr != 0)
      fatalerr(argv[0], 0, "chop Initialization failed\n");

     allargs[j].seed += j;
    pthrerr = pthread_create(alltids + j, NULL, philosopher, allargs + j);

    if (pthrerr != 0)
      fatalerr(argv[0], pthrerr, "Philosopher creation failed\n");
  }


  // allargs[nphil] = allargs[0];
  // allargs[nphil].seed += nphil;
  // clk((void *)(allargs + (nphil)));

   clk((void *)(allargs));





  int k;
  for (k = 0; k < nphil; k++)
  {
    printf("tid in joining thread is %d\n", (int)alltids[k]);

    pthrerr = pthread_join(*(alltids + k), NULL);
    if (pthrerr != 0)
      fatalerr(argv[0], pthrerr, "Philosopher join failed\n");
  }

  // print chopsticks

  // i = nphil - 1;
  // while (i >= 0)
  // {
  //   printf("i is %d,chop is %d\n", i, chopstick_state[i]);
  //   i--;
  // }
  exit(-1);
}

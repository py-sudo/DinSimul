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

/* default values for args */

int N = 5, T = 100;
double lam = 0.1, mu = 0.2;
int nblocked; /* The number of threads blocked */

/*utility functions */

int args_cmdline(int argc, char *argv[]);


/***********************************************************************
                         P H I L O S O P H E R
************************************************************************/
void *philosopher(void *vptr)
{
  unsigned int seed; /* This is called from main without    */
  int pthrerr;       /* creating a new thread               */
  struct thread_arg *ptr;

  ptr = (struct thread_arg *)vptr;

  while (1)
  {
    fatalerr("philosopher", 0, "Not yet implemented");
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

  fatalerr("server", 0, "Not yet implemented");
  exit(0);
}



/***********************************************************************
                               M A I N
************************************************************************/
int main(int argc, char **argv)
{


  args_cmdline(argc,argv);

  int pthrerr, i;
  int nserver, nclient, nticks = T, nphil = N;
  // double lam = lam
  // , mu = mu;
/*
    pthread_t tid; 
    int x = pthread_create (&tid, blah, blah, blah);

 */


  pthread_t tid;
  pthread_cond_t cv,chair,start_line; // all phil thread block on start_line condition var.

  pthread_mutex_t mutex;


  /*
  struct thread_arg
{
  int N, T;
  float lam, mu;
  pthread_cond_t *cv;
  pthread_mutex_t *blah;
  unsigned int seed;		Needed for the random number genarator 
};
*/
  
  struct thread_arg *allargs;
  allargs->N = N;
  allargs->lam = lam;
  allargs->mu = mu;
  allargs->cv = cv;
  allargs->mutex = PTHREAD_MUTEX_INITIALIZER;
  allargs->seed = 1;





  pthread_t *alltids;

  

  // printf("Should not be here\n");
  // printf("lamuda is %f \n", lam);
  //  printf("mu is %f \n", mu);
  // printf("T is %d \n", nticks);
  // printf("N is %d \n", nphil);
  exit(-1);
}

int args_cmdline(int argc, char *argv[])
{
  for (int i = 0; i < argc; i++)
  {
    if (strcmp(argv[i], "-L") == 0)
    {
      lam = atof(argv[i + 1]);
    }
    if (strcmp(argv[i], "-M") == 0)
    {
      mu = atof(argv[i + 1]);
    }
    if (strcmp(argv[i], "-T") == 0)
    {
      T = atof(argv[i + 1]);
    }
    if (strcmp(argv[i], "-N") == 0)
    {
      N = atof(argv[i + 1]);
    }
  }
}


/***********************************************************************/
/**      Author: Peiyi Guan                                      **/
/**        Date: Jul. 2019                                         **/
/***********************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "args.h"
#include "error.h"

int N=5, T=100;
double lam=0.2, mu=0.1;
int nblocked = 0;		/* The number of threads blocked on CLK*/
int nchblocked=0;		/* The number of threads blocked on
				   chopsticks or the chairs */
int neating=0, nthinking=0;	/* Used for collecting stats */
int nhungry=0;
int teating=0, tthinking=0, thungry=0;

/***********************************************************************
                         P H I L O S O P H E R
************************************************************************/
void *philosopher(void *vptr)
{
  unsigned int seed;		/* This is called from main without    */
  int pthrerr;			/* creating a new thread               */
  struct thread_arg *ptr;
  int eating=0;

  ptr = (struct thread_arg*)vptr;

  if (0!=(pthrerr=pthread_mutex_lock(ptr->maintex)))
    fatalerr("philosopher", pthrerr, "maintex lock failed");
  nblocked++;
  if (nblocked>=ptr->N) {
    if (0!=(pthrerr=pthread_cond_signal(ptr->clk_cond)))
     fatalerr("philosopher", pthrerr, "clk_cond (at start_line) signal failed");
  }
  if (0!=(pthrerr=pthread_cond_wait(ptr->start_line, ptr->maintex)))
    fatalerr("philosopher", pthrerr, "start_line wait failed");
  if (0!=(pthrerr=pthread_mutex_unlock(ptr->maintex)))
    fatalerr("philosopher", pthrerr, "maintex unlock failed");

  while (1) {
    if (0!=(pthrerr=pthread_mutex_lock(ptr->maintex)))
      fatalerr("philosopher", pthrerr, "maintex lock failed");
    if (!eating) {
      if (rand0_1(&(ptr->seed))<ptr->lam) {
	nthinking--;
	nhungry++;
	--*(ptr->chair_avail);
	while (*(ptr->chair_avail)<0) {
	  nchblocked++;
	  if (nblocked+nchblocked>=ptr->N)
	    if (0!=(pthrerr=pthread_cond_signal(ptr->clk_cond)))
	      fatalerr("philosopher", pthrerr, "clk_cond signal failed");
	  if (0!=(pthrerr=pthread_cond_wait(ptr->chair_cond, ptr->maintex)))
	    fatalerr("philosopher", pthrerr, "chair_cond wait failed");
	}
	
	--*(ptr->lchop_avail);
	while (*(ptr->lchop_avail)<0) {
	  nchblocked++;
	  if (nblocked+nchblocked>=ptr->N)
	    if (0!=(pthrerr=pthread_cond_signal(ptr->clk_cond)))
	      fatalerr("philosopher", pthrerr, "clk_cond signal failed");
	  if (0!=(pthrerr=pthread_cond_wait(ptr->lchop_cond, ptr->maintex)))
	    fatalerr("philosopher", pthrerr, "lchop_cond wait failed");
	}
	
	--*(ptr->rchop_avail);
	while (*(ptr->rchop_avail)<0) {
	  nchblocked++;
	  if (nblocked+nchblocked>=ptr->N)
	    if (0!=(pthrerr=pthread_cond_signal(ptr->clk_cond)))
	      fatalerr("philosopher", pthrerr, "clk_cond signal failed");
	  if (0!=(pthrerr=pthread_cond_wait(ptr->rchop_cond, ptr->maintex)))
	    fatalerr("philosopher", pthrerr, "rchop_cond wait failed");
	}
	eating=1;
	neating++;
	nhungry--;
      }
    } else {
      if (rand0_1(&(ptr->seed))<ptr->mu) { /* fi we are eating... */
	neating--;
	nthinking++;
	++*(ptr->chair_avail);
	if (*(ptr->chair_avail)<=0) {
	  nchblocked--;
	  if (0!=(pthrerr=pthread_cond_signal(ptr->chair_cond)))
	    fatalerr("philosopher", pthrerr, "chair_cond signal failed");
	}
	
	++*(ptr->lchop_avail);
	if (*(ptr->lchop_avail)<=0) {
	  nchblocked--;
	  if (0!=(pthrerr=pthread_cond_signal(ptr->lchop_cond)))
	    fatalerr("philosopher", pthrerr, "lchop_cond signal failed");
	}
	
	++*(ptr->rchop_avail);
	if (*(ptr->rchop_avail)<=0) {
	  nchblocked--;
	  if (0!=(pthrerr=pthread_cond_signal(ptr->rchop_cond)))
	    fatalerr("philosopher", pthrerr, "rchop_cond signal failed");
	}
	eating=0;
      }
    }
    nblocked++;
    if (nblocked+nchblocked>=ptr->N)
      if (0!=(pthrerr=pthread_cond_signal(ptr->clk_cond)))
	fatalerr("philosopher", pthrerr, "clk_cond signal failed");
    if (0!=(pthrerr=pthread_cond_wait(ptr->thr_cond, ptr->maintex)))
      fatalerr("philosopher", pthrerr, "thr_cond wait failed");
    if (0!=(pthrerr=pthread_mutex_unlock(ptr->maintex)))
      fatalerr("philosopher", pthrerr, "maintex unlock failed");
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

  ptr = (struct thread_arg*)vptr;

  if (0!=(pthrerr=pthread_mutex_lock(ptr->maintex)))
    fatalerr("clock", pthrerr, "maintex lock failed");

  while (nblocked<N) {
    if (0!=(pthrerr=pthread_cond_wait(ptr->clk_cond, ptr->maintex)))
      fatalerr("clk", pthrerr, "clk_cond (at start_line) wait failed");
  }
  if (0!=(pthrerr=pthread_cond_broadcast(ptr->start_line)))
      fatalerr("clk", pthrerr, "start_line signal failed");
  if (0!=(pthrerr=pthread_mutex_unlock(ptr->maintex)))
    fatalerr("clock", pthrerr, "maintex unlock failed");

  for (tick=0; tick<ptr->T; tick++) {
    if (0!=(pthrerr=pthread_mutex_lock(ptr->maintex)))
      fatalerr("clock", pthrerr, "maintex lock failed");
    while (nblocked+nchblocked<ptr->N) {
      if (0!=(pthrerr=pthread_cond_wait(ptr->clk_cond, ptr->maintex)))
	fatalerr("clk", pthrerr, "clk_cond wait failed");
    }
    tthinking+=nthinking;
    thungry  +=nhungry;
    teating  +=neating;
    nblocked=0;
    if (0!=(pthrerr=pthread_cond_broadcast(ptr->thr_cond)))
      fatalerr("clk", pthrerr, "thr_cond signal failed");
    if (0!=(pthrerr=pthread_mutex_unlock(ptr->maintex)))
      fatalerr("clock", pthrerr, "maintex unlock failed");
  }
  
  if (0!=(pthrerr=pthread_mutex_lock(ptr->maintex)))
    fatalerr("clock", pthrerr, "maintex lock failed");
  while (nblocked+nchblocked<ptr->N) {
    if (0!=(pthrerr=pthread_cond_wait(ptr->clk_cond, ptr->maintex)))
      fatalerr("clk", pthrerr, "clk_cond wait failed");
  }
}

/***********************************************************************
                               M A I N
************************************************************************/
int main(int argc, char **argv)
{
  int i, pthrerr;
  int chair_avail, nticks, *chop_avail;

  pthread_t *alltids;
  pthread_cond_t *chop_cond, clk_cond, thr_cond, start_line, chair_cond;
  pthread_mutex_t maintex, printex;
  struct thread_arg *allargs;

  i=1;
  while (i<argc-1) {
    if (strncmp("-L",argv[i],strlen(argv[i]))==0)
      lam     = atof(argv[++i]);
    else if (strncmp("-M",argv[i],strlen(argv[i]))==0)
      mu      = atof(argv[++i]);
    else if (strncmp("-N",argv[i],strlen(argv[i]))==0)
      N = atoi(argv[++i]);
    else if (strncmp("-T",argv[i],strlen(argv[i]))==0)
      T  = atoi(argv[++i]);
    else
      fatalerr(argv[i], 0, "Invalid argument\n");
    i++;
    }
  if (i!=argc)
    fatalerr(argv[0], 0, "Odd number of args\n");
  nthinking=N;

  allargs = (struct thread_arg *)malloc((N+1)*sizeof(struct thread_arg));
  if (allargs==NULL) fatalerr(argv[0], 0,"Out of memory (allargs)");
  alltids = (pthread_t*)malloc(N*sizeof(pthread_t));
  if (alltids==NULL) fatalerr(argv[0], 0,"Out of memory (alltids)");
  chop_avail = (int *)malloc(N*sizeof(int));
  if (chop_avail==NULL) fatalerr(argv[0], 0,"Out of memory (chop_avail)");
  chop_cond = (pthread_cond_t *)malloc(N*sizeof(pthread_cond_t));
  if (chop_cond==NULL) fatalerr(argv[0], 0,"Out of memory (chop_cond)");
  
  chop_cond = (pthread_cond_t *)malloc(N*sizeof(pthread_cond_t));
  if (chop_cond==NULL) fatalerr(argv[0], 0,"Not enough memory (chop_cond)");
  for (i=0; i<N; i++){
    if (0!=(pthrerr=pthread_cond_init(chop_cond+i,NULL)))
      fatalerr(argv[0], 0,"Initialization of chop_cond failed");
    chop_avail[i]=1;
  }
  chair_avail = N-1;
  if (0!=(pthrerr=pthread_cond_init(&clk_cond, NULL)))
     fatalerr(argv[0], 0,"Initialization of clk_cond failed");
  if (0!=(pthrerr=pthread_cond_init(&thr_cond, NULL)))
     fatalerr(argv[0], 0,"Initialization of thr_cond failed");
  if (0!=(pthrerr=pthread_cond_init(&chair_cond, NULL)))
     fatalerr(argv[0], 0,"Initialization of chair_cond failed");
  if (0!=(pthrerr=pthread_cond_init(&start_line, NULL)))
     fatalerr(argv[0], 0,"Initialization of start_line failed");

  if (0!=(pthrerr=pthread_mutex_init(&maintex, NULL)))
    fatalerr(argv[0], 0,"Initialization of maintex  failed\n");
  if (0!=(pthrerr=pthread_mutex_init(&printex, NULL)))
    fatalerr(argv[0], 0,"Initialization of printex  failed\n");

  allargs[0].me         = -1;
  allargs[0].N          = N;
  allargs[0].T          = T;
  allargs[0].lam        = lam;
  allargs[0].mu         = mu;
  allargs[0].clk_cond   = &clk_cond;
  allargs[0].thr_cond   = &thr_cond;
  allargs[0].start_line = &start_line;
  allargs[0].chair_cond = &chair_cond;
  allargs[0].maintex    = &maintex;
  allargs[0].printex    = &printex;
  allargs[0].chair_avail= &chair_avail;
  allargs[0].lchop_avail= NULL;
  allargs[0].rchop_avail= NULL;
  allargs[0].lchop_cond = NULL;
  allargs[0].rchop_cond = NULL;
  allargs[0].seed       =0;
  for (i=0; i<N; i++){
    allargs[i] = allargs[0];
    allargs[i].seed+=i;
    allargs[i].me          = i;
    allargs[i].lchop_avail = &(chop_avail[i]);
    allargs[i].rchop_avail = &(chop_avail[(i+1)%N]);
    allargs[i].lchop_cond  = &(chop_cond[i]);
    allargs[i].rchop_cond  = &(chop_cond[(i+1)%N]);
    if (0!=(pthrerr=pthread_create(alltids+i,NULL,philosopher,allargs+i)))
      fatalerr(argv[0], pthrerr,"Philosopher creation failed\n");
  }
  allargs[N] = allargs[0];
  allargs[N].seed+=i;
  allargs[N].me          = -1;
  clk((void*)(allargs+N));
  printf("Average Thinking Time: %6.2f\n", (double)tthinking/(double)(N*T));
  printf("Average Hungry Time: %6.2f\n", (double)thungry/(double)(N*T));
  printf("Average Eating Time: %6.2f\n", (double)teating/(double)(N*T));
  exit(0);
}

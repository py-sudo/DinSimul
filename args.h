struct thread_arg
{
  int N, T,state,index;
  float lam, mu;
  pthread_cond_t *start_line,*sclkblock;	/* find a better name */
  pthread_mutex_t *mutex;
  unsigned int seed;		/* Needed for the random number genarator */
};

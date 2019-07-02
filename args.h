struct thread_arg
{
  int N, T;
  float lam, mu;
  pthread_cond_t *cv;	/* find a better name */
  pthread_mutex_t *mutex;
  unsigned int seed;		/* Needed for the random number genarator */
};

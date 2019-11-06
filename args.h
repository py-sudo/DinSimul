/***********************************************************************/
/**      Author: Peiyi Guan                                      **/
/**        Date: Jul. 2019                                         **/
/***********************************************************************/


struct thread_arg
{
  int me;
  int N, T;
  double lam, mu;
  pthread_cond_t *clk_cond, *thr_cond, *start_line, *chair_cond;
  pthread_mutex_t *maintex, *printex;
  int *lchop_avail, *rchop_avail, *chair_avail;
  pthread_cond_t *lchop_cond, *rchop_cond;
  unsigned int seed;		/* Needed for the random number genarator */
};

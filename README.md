

# What is Thread Simulator
In this pthread program the main program creates ***N*** threads that behave as philosophers. All of them immediately block on condition variable ```start_line```. 

After all threads are created, the main thread wakes them up and then waits until the all of them have completed one cycle (or were blocked waiting for a chopstick or a chair).

The main thread then collects some statistics and lets the philosopher run one more cycle for a total of ***T*** cycles . After the last cycle the main thread prints out the statics it collected and exits.

Every philosopher after waking from the condition variable ```start_line``` starts executing cycles. 

Each philosopher can be in one of three states: ```THINK```, ```HUNGRY``` or ```EATING```. 

If he is in ```THINK``` state he invokes a random number generator (which generates numbers between 0 and 1 uniformly distributed) and compares the result with ***l***. If the random number is less than ***l*** he switches to ```HUNGRY```.

If he is in ```HUNGRY``` state he will switch to ```EATING``` state when the chopsticks become available. And if the philosopher is in ```EATING``` state he invokes the random number generator and
compares the result with ***μ***. If the random number is less than ***μ*** he switches back to ```THINK``` state.

To avoid deadlock the philosophers limit the number of philosophers contending for the chopsticks to ```N-1``` using a chair condition variable. You need one mutex to synchronize access to shared variables and blocking on condition variables.

> The main thread at the end of every cycle collects the apropriate statistics and reports the following:

> Average Thinking Time: It does it by summing up the thinking time of all the philosophers and dividing by ***N***. 

> Average Hungry Time : It does it by summing up the hungry time of all the philosophers and dividing by ***N***. 

> Average Eating Time : It does it by summing up the eating time of all the philosophers and dividing by ***N***.

# How to run it

Use ```make``` command to compile an c program then run it like this: 
```./simul -N 6 -L .9 -M 0.10 -T 100000```

> -N N where N is the number of philosophers.
-T T where T is the number of clock ticks.
-L L where L is the λ.
-M M where M is the µ

# What I Learned
Solved classic ***Dining philosophers problem*** using multi-threading,mutex and conditional variables.
pthread functions used here:

>* ```pthread_create```
>* ```pthread_cond_init```
>* ```pthread_cond_wait```
>* ```pthread_cond_signal```
>* ```pthread_cond_broadcast```
>* ```pthread_mutex_init```
>* ```pthread_mutex_lock```
>* ```pthread_mutex_unlock```

# License
[MIT](https://choosealicense.com/licenses/mit/)
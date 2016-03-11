/* File:     semaphores.c
 * Author:    Cayla Shaver
 * Section:   2
 *
 * Purpose:  Lock and unlock a semaphore many times, and report on elapsed time
 *
 * Compile:  gcc -g -Wall -o semaphores semaphores.c -lpthread
 * Run:      ./semaphores <thread_count> <n>
 *              n:  number of times the semaphore is locked and unlocked
 *                  by each thread
 *
 * Input:    none
 * Output:   Total number of times semaphore was locked and elapsed time for
 *           the threads
 * Run time: Semaphores:   On the Penguin cluster with 4 threads and n = 1000000
 *           Run 1:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 3.948181e+00 seconds
 *           Run 2:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 3.601990e+00 seconds
 *           Run 3:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 3.693148e+00 seconds
 *           The fastest runtime recorded for semaphores.c was 3.601990e+00 seconds
 *
 * Run time: Mutexes:   On the Penguin cluster with 4 threads and n = 1000000
 *           Run 1:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 9.698188e-01 seconds
 *           Run 2:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 1.020545e+00 seconds
 *           Run 3:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 1.097897e+00 seconds
 *           The fastest runtime recorded for many_mutexes.c was 9.698188e-01 seconds
 *
 *
 * Run time: Semaphores:   On the Lab Computer with 4 threads and n = 1000000
 *           Run 1:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 1.008096e+00 seconds
 *           Run 2:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 1.013990e+00 seconds
 *           Run 3:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 9.329059e-01 seconds
 *           The fastest runtime recorded for semaphores.c was 9.329059e-01 seconds
 *
 * Run time: Mutexes:   On the Lab Computer with 4 threads and n = 1000000
 *           Run 1:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 5.843592e-01 seconds
 *           Run 2:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 5.748520e-01 seconds
 *           Run 3:  Total number of times semaphore was locked and unlocked: 4000000
 *           Elapsed time = 5.889380e-01 seconds
 *           The fastest runtime recorded for many_mutexes.c was 5.748520e-01 seconds
 *             
 *           I have concluded that using the penguin cluster using semaphores is much faster
 *           than the mutexes.
 *           Also I have found that if not run on the penguin cluster just the machine 
 *           that the mutexes are faster than the semaphores
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"
#include <semaphore.h>

int thread_count;
int n;
int total = 0;
sem_t semi;

void Usage(char prog_name[]);
void* Lock_and_unlock(void* rank);

int main(int argc, char* argv[]) {
   pthread_t* thread_handles;
   long thread;
   double start, finish;

   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   n = strtol(argv[2], NULL, 10);

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   sem_init(&semi, 0, 1);

   GET_TIME(start);
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL, Lock_and_unlock,
            (void*) thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);
   GET_TIME(finish);

   printf("Total number of times semaphore was locked and unlocked: %d\n",
         total);
   printf("Elapsed time = %e seconds\n", finish-start);

   sem_destroy(&semi);
   free(thread_handles);
   return 0;
}  /* main */

/*---------------------------------------------------------------------
 * Function:   Usage
 * Purpose:    Print a message explaining how to start the program.
 *             Then quit.
 * In arg:     prog_name:  name of program from command line
 */
void Usage(char prog_name[]) {
   fprintf(stderr, "usage: %s <thread_count> <n>\n", prog_name);
   fprintf(stderr, "    n: number of times semaphore is locked and ");
   fprintf(stderr, "unlocked by each thread\n");
   exit(0);
}  /* Usage */


/*---------------------------------------------------------------------
 * Function:   Lock_and_unlock
 * Purpose:    Repeatedly lock and unlock a semaphore to determine performance
 *             of semaphores
 * In arg:     rank:  thread rank
 * In globals: thread_count:  number of threads
 *             n:  number of times each thread should lock and unlock semaphore 
 *             semaphore:
 * In/out global:  total:  total number of times semaphore is locked and unlocked.
 */
void* Lock_and_unlock(void* rank) {
   //long my_rank = (long) rank;  /* unused */
   int i;

   for (i = 0; i < n; i++) {
      sem_wait(&semi);
      total++;
      sem_post(&semi);
   }

   return NULL;
}  /* Lock_and_unlock */
/* File:     parallelPrimes.c
 * Author:   Cayla Shaver
 * Section:  2
 * Purpose:  Find all primes less than or equal to an input value.
 *           This version doesn't bother checking even ints.
 *
 * Input:    n:  integer >= 2 (from command line)
 * Output:   Sorted list of primes between 2 and n,
 *
 * Compile:  mpicc -g -Wall -o p p4.c -lm
 * Usage:    mpiexec -n 4 p 10
 *              n:  max int to test for primality
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>


int Is_prime(int i);
void Merge(int** master, int* master_count, int your_primeHolder[], int recv_count, int** temp);
void Global_List(int** master, int c, int my_rank, int p, int n, MPI_Comm comm, int tpc);
void Print_master_list(int master[], int total);


int main(int argc, char* argv[]) {
   int n, i, p, my_rank;
   int* primeHolder;
   MPI_Comm comm;
   long max;
   int count = 0;
   int total_prime_count = 0;

   MPI_Init(&argc, &argv);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &p);
   MPI_Comm_rank(comm, &my_rank);

   if(my_rank == 0){
      n = strtol(argv[1], NULL, 10);
   }
   MPI_Bcast(&n, 1, MPI_INT, 0, comm);

   max = n / (2 * p) + 2;

   primeHolder = malloc(max *sizeof(int));
   for (i = 2 * my_rank + 3; i < n; i += 2 * p){
     if (Is_prime(i)){
         primeHolder[count] += i;
         count += 1;
     } 
   }


     
   Global_List(&primeHolder, count, my_rank, p, n, comm,
         total_prime_count);
   

   if (my_rank == 0)
      Print_master_list(primeHolder, total_prime_count);

   //free(master);
   free(primeHolder);
   MPI_Finalize();
   return 0;
}  /* main */

/*-------------------------------------------------------------------
 * Function:   Is_prime
 * Purpose:    Determine whether the argument is prime
 * Input arg:  i
 * Return val: true (nonzero) if arg is prime, false (zero) otherwise
 */
int Is_prime(int i) {
   int j;

   for (j = 2; j <= sqrt(i); j++)
      if (i % j == 0)
         return 0;
   return 1;
}  /* Is_prime */


/*-------------------------------------------------------------------
 * Function:   Merge
 * Purpose:    Merge the contents of the arrays A and B into array C
 * Input args:
 *    curr_m_size:  the number of elements in A
 *    your_size:  the number of elements in B
 *    csize:  the number of elements in C (= asize + bsize)
 *    master, your_primeHolder:  the arrays
 *
 * Output arg:
 *    temp:  result array
 */
void Merge(int** master, int* curr_m_size, int your_primeHolder[], int your_size, int** temp) {
   int ai, bi, ci;
   int csize = (*curr_m_size) + your_size;
   
   ai = bi = ci = 0;
   while (ai < *curr_m_size && bi < your_size) {
      if ((*master)[ai] <= your_primeHolder[bi]) {
         (*temp)[ci] = (*master)[ai];
         ci++; ai++;
      } else {
         (*temp)[ci] = your_primeHolder[bi];
         ci++; bi++;
      }
   }

   if (ai >= *curr_m_size){
      for (; ci < csize; ci++, bi++){
         (*temp)[ci] = your_primeHolder[bi];
      }
   }
   else{
      for (; ci < csize; ci++, ai++){
         (*temp)[ci] = (*master)[ai];
      }
   }
}  /* Merge */


/*---------------------------------------------------------------
 * Function:  Global_List
 * Purpose:   Take individual lists from each process and add them to
 *            the master list
 * Input args:
 *    my_primeHolder:  the calling process' list of prime #s
 *    c: 		size of my_primeHolder
 *    my_rank:     the calling process' rank in the communicator
 *    p:           the number of processes in the communicator
 *    comm:        the communicator used for sends and receives
 *    tpc:			the total prime counter
 *
 * Algorithm:  Use tree structured communication, pairing processes
 *    to communicate.           
 */

void Global_List(int** my_primeHolder, int c, int my_rank, int p, int n, MPI_Comm comm, int tpc) {
    int partner, recv_count;
    int* your_primeHolder;
    int* temp;
    int done = 0;
    unsigned bitmask = (unsigned) 1;
    int curr_master_size = c;
    MPI_Status status;



    MPI_Allreduce(&curr_master_size, &tpc, 1, MPI_INT, MPI_SUM, comm);
    your_primeHolder = malloc(tpc*sizeof(int));
    temp = malloc(tpc*sizeof(int));

#   ifdef DEBUG
    int my_pass = -1;
    partner = -1;
    printf("Proc %d > partner = %d, bitmask = %d, pass = %d\n", 
        my_rank, partner, bitmask, my_pass);
    fflush(stdout);
#   endif
    //for (i = 0; i < c; i++)
       //master[i] = my_primeHolder[i];



    while (!done && bitmask < p) {
        partner = my_rank ^ bitmask;
#       ifdef DEBUG
        my_pass++;
        printf("Proc %d > partner = %d, bitmask = %d, pass = %d\n", 
           my_rank, partner, bitmask, my_pass);
        fflush(stdout);
#       endif
        if (my_rank < partner) {
            if (partner < p) {
                MPI_Recv(your_primeHolder, tpc, MPI_INT, partner, 0, comm, 
                      &status);

                MPI_Get_count(&status, MPI_INT, &recv_count);

                Merge(my_primeHolder, &curr_master_size, your_primeHolder, recv_count, &temp);
                curr_master_size += recv_count;
            }
            bitmask <<= 1;
        } else {
            MPI_Send(my_primeHolder, tpc, MPI_INT, partner, 0, comm); //master total size or tpc
            done = 1;
        }

    }

    free(temp);
    free(your_primeHolder);
    /* Valid only on 0 */

}  /* Global_List */


/*-------------------------------------------------------------------
 * Function:   Print_master_list
 * Purpose:    
 * Input arg:  
 */
void Print_master_list(int master[], int total){
  int i;

  for(i = 0; i <= total; i ++){
    printf("%d\n", master[i]);
  }
}  /* Print_master_list */


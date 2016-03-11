/* File:       mpi_hello0.c
 * Author:     Cayla Shaver
 * Section:    2
 *
 * Purpose:    A "hello,world" program that uses MPI
 *
 * Compile:    mpicc -g -Wall -o mpi_hello0 mpi_hello0.c
 * Run:        mpiexec -n<number of processes> ./mpi_hello0
 *
 * Input:      None
 * Output:     A greeting from each process to the next higher ranked
 *             process.
 *
 * Algorithm:  Each process sends a message to process 0,
 *             which prints the messages it has received,
 *             as well as its own message.
 */
#include <stdio.h>
#include <string.h>  /* For strlen             */
#include <mpi.h>     /* For MPI functions, etc */

const int MAX_STRING = 100;

int main(void) {
   char       greeting[MAX_STRING];
   int        my_rank, p;
   int        dest, source;

   /* Start up MPI */
   MPI_Init(NULL, NULL);

   /* Get the number of processes */
   MPI_Comm_size(MPI_COMM_WORLD, &p);

   /* Get my rank among all the processes */
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   dest = (my_rank + 1) % p;
   source = (my_rank + p - 1) % p;

   if (p == 1) {
      printf("Argh I be from %d. \n", my_rank);
   } else {
      sprintf(greeting, "Argh I be from %d ", 
            my_rank);
      MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, dest, 0,
            MPI_COMM_WORLD);
      MPI_Recv(greeting, MAX_STRING, MPI_CHAR, source, 
            0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("Argh this be my process %d received a message: %s \n", 
         my_rank, greeting);

   }

   /* Shut down MPI */
   MPI_Finalize();
   return 0;
}  /* main */
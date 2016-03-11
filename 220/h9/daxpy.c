/* File:     
 *     pth_mat_vect.c 
 *
 * Author: Cayla Shaver
 * Section: 2
 * Purpose:  
 *     Computes a parallel matrix-vector product.  Matrix
 *     is distributed by block rows.  Vectors are distributed by 
 *     blocks.
 *
 * Input:
 *     m, n: order of matrix
 *     A, x: the matrix and the vector to be multiplied
 *
 * Output:
 *     y: the product vector
 *
 * Compile:  gcc -g -Wall -o pth_mat_vect pth_mat_vect.c -lpthread
 * Usage:
 *     pth_mat_vect <thread_count>
 *
 * Notes:  
 *     1.  Local storage for A, x, y is dynamically allocated.
 *     2.  Number of threads (thread_count) should evenly divide both 
 *         m and n.  The program doesn't check for this.
 *     3.  We use a 1-dimensional array for A and compute subscripts
 *         using the formula A[i][j] = A[i*n + j]
 *     4.  Distribution of A, x, and y is logical:  all three are 
 *         globally shared.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Global variables */
int     thread_count;
int     n;
double  alpha;
double* x;
double* y;

/* Serial functions */
void Usage(char* prog_name);
void Read_array(char* prompt, double x[], int n);
void Print_array(char* title, double y[], int n);

/* Parallel function */
void *Pth_array(void* rank);

/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   thread_handles = malloc(thread_count*sizeof(pthread_t));

   printf("Enter n and alpha\n");
   scanf("%d%lf", &n, &alpha);

   
   x = malloc(n*sizeof(double));
   y = malloc(n*sizeof(double));
   
   Read_array("Enter array for x", x, n);
   Print_array("We read", x, n);

   Read_array("Enter array for y", y, n);
   Print_array("We read", y, n);

   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
         Pth_array, (void*) thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);

   Print_array("The product is", y, n);

   
   free(x);
   free(y);
   free(thread_handles);

   return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function:        Read_array
 * Purpose:         Read in the vector x
 * In arg:          prompt, n
 * Out arg:         x
 */
void Read_array(char* prompt, double x[], int n) {
   int   i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++) 
      scanf("%lf", &x[i]);
}  /* Read_array */


/*------------------------------------------------------------------
 * Function:       Pth_array
 * Purpose:        Multiply an mxn matrix by an nx1 column vector
 * In arg:         rank
 * Global in vars: A, x, y, n, thread_count
 * Global out var: y
 */
void *Pth_array(void* rank) {
   long my_rank = (long) rank;
   int i;
   int local_n = n/thread_count; 
   int my_first_row = my_rank*local_n;
   int my_last_row = my_first_row + local_n - 1;

   for (i = my_first_row; i <= my_last_row; i++) {          
      y[i] += alpha*x[i];
   }

   return NULL;
}  /* Pth_array */


/*------------------------------------------------------------------
 * Function:    Print_array
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void Print_array(char* title, double y[], int m) {
   int   i;

   printf("%s\n", title);
   for (i = 0; i < m; i++)
      printf("%4.1f ", y[i]);
   printf("\n");
}  /* Print_array */
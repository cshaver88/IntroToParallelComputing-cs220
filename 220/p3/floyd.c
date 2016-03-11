/* File:     floyd.c
 * Author:   Cayla Shaver
 * Section:  2
 * Purpose:  Implement Floyd's algorithm for solving the all-pairs shortest
 *           path problem:  find the length of the shortest path between each
 *           pair of vertices in a directed graph.
 *
 * Input:    n, the number of vertices in the digraph
 *           mat, the adjacency matrix of the digraph
 * Output:   A matrix showing the costs of the shortest paths
 *
 * Compile:  mpicc -g -Wall -o pfloyd parallelFloyd.c
 *           (See note 7)
 * Run:      ./pfloyd
 *           For large matrices, put the matrix into a file with n as
 *           the first line and run with ./floyd < large_matrix
 *
 * Notes:
 * 1.  The input matrix is overwritten by the matrix of lengths of shortest
 *     paths.
 * 2.  Edge lengths should be nonnegative.
 * 3.  If there is no edge between two vertices, the length is the constant
 *     INFINITY.  So input edge length should be substantially less than
 *     this constant.
 * 4.  The cost of travelling from a vertex to itself is 0.  So the adjacency
 *     matrix has zeroes on the main diagonal.
 * 5.  No error checking is done on the input.
 * 6.  The adjacency matrix is stored as a 1-dimensional array and subscripts
 *     are computed using the formula:  the entry in the ith row and jth
 *     column is mat[i*n + j]
 * 7.  Use the compile flag -DSHOW_INT_MATS to print the matrix after its
 *     been updated with each intermediate city.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

const int INFINITY = 1000000;

void Read_matrix(int mat[], int n);
void Print_matrix(int mat[], int n);
void Floyd(int local_mat[], int n, int p, int my_rank);
int min(int m, int k);

int main(void) {
   int  n;
   int p;
   int my_rank;
   int* mat = NULL;
   int* local_mat;
   MPI_Comm comm;

   MPI_Init(NULL, NULL);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &p);
   MPI_Comm_rank(comm, &my_rank);

   if(my_rank == 0){
      printf("How many vertices?\n");
      scanf("%d", &n);
      mat = malloc(n* n * sizeof(int));
      printf("Enter the matrix\n");
      Read_matrix(mat, n);
   }
   MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
   local_mat = malloc(n * (n/p) * sizeof(int));
   MPI_Scatter(mat, n * n / p, MPI_INT, local_mat, n * n / p, MPI_INT, 0, comm);
   Floyd(local_mat, n, p, my_rank);
   MPI_Gather(local_mat, n * n / p, MPI_INT, mat, n * n / p, MPI_INT, 0, comm);
   if(my_rank == 0){
         printf("The solution is:\n");
   		 Print_matrix(mat, n);
   }
   free(local_mat);
   free(mat);

   MPI_Finalize();
   return 0;
}  /* main */

/*-------------------------------------------------------------------
 * Function:  Read_matrix
 * Purpose:   Read in the adjacency matrix
 * In arg:    n
 * Out arg:   mat
 */
void Read_matrix(int mat[], int n) {
   int i, j;

   for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
         scanf("%d", &mat[i*n+j]);
}  /* Read_matrix */

/*-------------------------------------------------------------------
 * Function:  Print_matrix
 * Purpose:   Print the contents of the matrix
 * In args:   mat, n
 */
void Print_matrix(int mat[], int n) {
   int i, j;

   for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++)
         if (mat[i*n+j] == INFINITY)
            printf("i ");
         else
            printf("%d ", mat[i*n+j]);
      printf("\n");
   }
}  /* Print_matrix */

/*-------------------------------------------------------------------
 * Function:    Floyd
 * Purpose:     Apply Floyd's algorithm to the matrix mat
 * In arg:      n
 * In/out arg:  mat:  on input, the adjacency matrix, on output
 *              lengths of the shortest paths between each pair of
 *              vertices.
 */
void Floyd(int local_mat[], int n, int p, int my_rank) {
   int int_city, city2, root, local_int_city;
   int j, local_city1;
   int*  row_int_city;
   row_int_city = malloc(n * sizeof(int));

   for (int_city = 0; int_city < n; int_city++) {
      root = int_city / (n/p);
      if (my_rank == root){
         local_int_city = int_city % (n/p);
         for(j = 0; j < n; j ++)
            row_int_city[j] = local_mat[local_int_city * n + j];
      }
      MPI_Bcast(row_int_city, n, MPI_INT, root, MPI_COMM_WORLD);
      for (local_city1 = 0; local_city1 < n/p; local_city1 ++)
         for(city2 = 0; city2 < n; city2 ++)
            local_mat[local_city1 * n + city2] = 
               min(local_mat[local_city1 * n + city2], 
                  local_mat[local_city1 * n + int_city]
                     + row_int_city[city2]);
   }
}  /* Floyd */

/*-------------------------------------------------------------------
 * Function:  Min
 * Purpose:   Find the minimum value b/wn m and k
 * In arg:    m, k
 * Out arg:   m or k
 */
int min(int m, int k) {
	if (m < k){
		return m;
	}
	else{
		return k;
   }
}  /* Min */
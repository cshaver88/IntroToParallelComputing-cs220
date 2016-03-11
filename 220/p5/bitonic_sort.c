/* File:     bitonic_sort.c
 *
 * Author:   Cayla Shaver
 * Section:  2
 * Purpose:  
 *
 * Note:     n will be only a power of 2.
 *
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include <string.h>
 #include "timer.h"

 /* Global variables:  accessible to all threads */
 int thread_count; 
 const int Max = 999999;
 int size;
 int* list;
 pthread_t* actual_threads;
 int* temp;


 void Usage(char* prog_name);
 void* Phase_func(void* counter);
 int Sort(int* one, int* two);
 void Command_Line_Args(int argc, char* argv[]);
 void Find_Partner(long my_rank, int* temp_sublist, int block_partition);
 void Merge_split_low(int* my_list, int* partner_list, int* extra_list, 
        int block_partition, int rank);
 void Merge_split_high(int* my_list, int* partner_list, int* extra_list, 
        int block_partition, int rank);
 int* Random(int* list, int size);
 void Print(int *random_list, int size);

 /*--------------------------------------------------------------------*/
 int main(int argc, char* argv[]){
    long counter;
    double start, finish, total;
    int* temp;


    Command_Line_Args(argc, argv);
    Print(list, size);


    GET_TIME(start); 

    for (counter = 0; counter < thread_count; counter++) { 
       pthread_create(&actual_threads[counter], NULL,
          Phase_func, (void*) counter);  
    }

    temp = malloc(size * sizeof(int));

    for(counter = 0; counter < thread_count; counter++){
        pthread_join(actual_threads[counter], NULL);
    }

    free(actual_threads);

    GET_TIME(finish);
    total = (finish - start);
    printf("Total elapsed time for the program: %f milliseconds\n", total);

    return 0;
 }  /* main */

/*-------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     gives errors if there are an incorrect number of 
 *                 threads or bad arguments are given
 *
 * Input args:  program name of the file that is being passed in
 */
void Usage(char* prog_name) {
   fprintf(stderr, "Usage: %s <number of threads>\n", prog_name);
   fprintf(stderr, "Number of threads > 0\n");
   exit(0);
}  /* Usage */

/*-------------------------------------------------------------------
 * Function:    Phase_func
 * Purpose:     divide up the list between the threads then 
 *                perform a local sort in the threads
 *
 * Input args:  pointer to the counter for number of threads 
 *                from command line
 */
void* Phase_func(void* counter) {


    long rank = (long) counter;
    int block_partition = size / thread_count;
    int first = rank * block_partition;
    int i;
    int* temp_sublist = malloc(block_partition* sizeof(int));

    for (i = 0; i < block_partition; i++){
        temp_sublist[i] = list[i + first];
    }
    qsort(temp_sublist, block_partition, sizeof(int), (int (*)
        (const void *, const void *)) Sort);
    Find_Partner(rank, temp_sublist, block_partition);

    return NULL;

}  /* Phase_func */


/*-------------------------------------------------------------------
 * Function:    Sort
 * Purpose:     sort the elements it is given
 *
 * Input args:  pointers to two different elements in the global list
 */

int Sort(int* one, int* two) {

    if (*one > *two){
        return 1;
    }
    else if(*one < *two){
        return -1;
    }
    else{
        return 0;
    }

}  /* Sort */

/*-----------------------------------------------------------------
 * Function:    Command_Line_Args
 * Purpose:     Do all checks for g and o as well as make sure there 
 *              aren't any bad arguments ex: 
 *              (0 threads, or not enough arguments)
 *
 * Input args:  argc and argv command line arguments
 *
 */

void Command_Line_Args(int argc, char* argv[]){

    int i;

    srandom(1);

    if (argc < 3){ 
        Usage(argv[0]);
    } 
    
    thread_count = strtol(argv[1], NULL, 10);
    size = strtol(argv[2], NULL, 10);
    list = malloc(size * sizeof(int));
    actual_threads = malloc (thread_count * sizeof(pthread_t));

    if (thread_count <= 0){ 
        Usage(argv[0]);
    }
    if (argc == 4){ 
        if(*argv[3] == 'g'){
            list = Random(list, size);
        }
        else{
            Usage(argv[0]);
        }
    }
    else if (argc == 5){
        if(*argv[3] == 'g' && *argv[4] == 'o'){
            list = Random(list, size);
            Print(list, size);
        }
        else{
            Usage(argv[0]);
        }
    }
    else{
        printf("Please enter %d numbers.\n", size);
        for (i = 0; i < size; i ++){
            scanf("%d", &list[i]);
        }
    } 

 } /* Command_Line_Args */


/*-----------------------------------------------------------------
 * Function:    Find_Partner   
 *
 * Input args:  my_rank--rank of the thread
 *              temp_sublist--threads list
 *              block_partition--size of the threads sublist
 *   
 *
 * Notes:
 *    1.  Uses butterfly structured communication.
 *    2.  p, the number of processes must be a power of 2.
 *    3.  The pairing of the processes is done using bitwise
 *        and bitwise &.
 */
void Find_Partner(long my_rank, int* temp_sublist, int block_partition) {

    int*       extra_list = malloc(block_partition * sizeof(int));
    int*       partner_list = malloc(block_partition * sizeof(int));
    int        partner;
    unsigned   bitmask = 1;
    unsigned   bitmask_two = 1;
    unsigned   and_bit = 2;
    int i;

    while (bitmask < thread_count) {
        bitmask_two = bitmask;
        while (bitmask_two >= 1){ 
            partner = my_rank ^ bitmask;
            for (i = 0; i < block_partition; i++){
              partner_list[i] = list[i + (partner * block_partition)];
            }
            if((my_rank & and_bit) == 0){
                //increase butterfly
                if (my_rank < partner){
                    Merge_split_low(temp_sublist, partner_list, extra_list, block_partition, my_rank);//my list, partner list, temp list, 
                }
                else{
                    Merge_split_high(temp_sublist, partner_list, extra_list, block_partition, my_rank);
                }
            }
            else{
               //decreasing butterfly
                if(my_rank < partner){
                    Merge_split_high(temp_sublist, partner_list, extra_list, block_partition, my_rank);
                }
                else{
                    Merge_split_low(temp_sublist, partner_list, extra_list, block_partition, my_rank);
                }
            }
            bitmask_two >>= 1;
        }
        bitmask <<= 1;
        and_bit <<= 1;
    }
}  /* Find_Partner */

/*-------------------------------------------------------------------
 * Function:    Merge_split_low
 * Input args:  partner_list--list to be compared and added into 
 *                list
 *              extra_list--list to copy the compared lists to
 *              block_partition--size of the threads elements
 *              rank--thread rank
 *
 *
 */
void Merge_split_low(int* my_list, int* partner_list, int* extra_list, 
        int block_partition, int rank) {
   int my_index, your_index, our_index;
   
   my_index = 0;
   your_index = 0;
   our_index = 0;
   while (our_index < block_partition) {
      if (my_list[my_index] <= partner_list[your_index]) {
         extra_list[our_index] = my_list[my_index];
         our_index++; my_index++;
      } else {
         extra_list[our_index] = partner_list[your_index];
         our_index++; your_index++;
      }
   }
   memcpy(my_list, extra_list, block_partition*sizeof(int));
   for(i = 0; i < block_partition; i ++){
      list[rank * block_partition] = my_list[i];
   }
}  /* Merge_split_low */

/*-------------------------------------------------------------------
 * Function:    Merge_split_high
 * Input args:  my_list--threads' list
 *              partner_list--list to be compared and added into 
 *                list
 *              extra_list--list to copy the compared lists to
 *              block_partition--size of the threads elements
 *              rank--thread rank
 *
 */
void Merge_split_high(int* my_list, int* partner_list, int* extra_list, 
        int block_partition, int rank) { 
   int my_index, your_index, our_index;
   
   my_index = block_partition - 1;
   your_index = block_partition - 1;
   our_index = block_partition - 1;
   while (our_index >= 0) {
      if (my_list[my_index] >= partner_list[your_index]) {
         extra_list[our_index] = my_list[my_index];
         our_index --; my_index --;
      } else {
         extra_list[our_index] = partner_list[your_index];
         our_index --; your_index --;
      }
   }

   memcpy(my_list, extra_list, block_partition * sizeof(int));
   for(i = 0; i < block_partition; i ++){
      list[rank * block_partition] = my_list[i];
   }
}  /* Merge_split_high */


/*-----------------------------------------------------------------
 * Function:    Random
 * Purpose:     Create a list of random numbers
 *
 * Input args:  list to add the numbers to 
 *
 */
 int* Random(int *list, int size){
    int i;
    int random_elem;

    for (i = 0; i < size; i ++){
        random_elem = random() % Max;
        list[i] = random_elem;
    }
    return list;

 }  /* Random */

/*-----------------------------------------------------------------
 * Function:    Print
 * Purpose:     Print the list of numbers
 *
 * Input args:  list to print, and its size
 *
 */
 void Print(int *list, int size){
    int i;

    for (i = 0; i < size; i ++){
        printf("%d\n", list[i]);
    }
 }  /* Print */
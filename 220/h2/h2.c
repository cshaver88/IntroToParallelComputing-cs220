/* File:    h2.c
 * Author:  Cayla Shaver
 * Purpose: Read in an array of ints, print out the input array,
 *          reverse the order of the elements in the array. 
 *          Finally print out the updated array.
 *
 * Input:   n-ints, elems-elements in array.
 * Output:  original input array, the reversed array.
 *
 * Compile: gcc -g -Wall -o h2 h2.c
 * Run:     ./h2
 *
 * Note:    There will be no more than 20 elements.
 */

#include <stdio.h>

void Reverse(int size, int list[]);
void PrintList(int list[], int size);

int main(void) {

	 //int size;                  /*number of elements in the array*/
    printf("Enter a number less than 20\n");
    int size;
    scanf("%d", &size);
   printf("Enter a list of numbers, one less than the number you entered\n");
   
   int list[size];     
   int i; 

    for(i = 0; i < size; i ++){
      scanf("%d", &list[i]);
    }

   printf("The input list is \n");
   PrintList(list, size);

   Reverse(size, list);//reverse the list 

   printf("The reverse list is \n");
   PrintList(list, size);

   return 0;
}  /* main */



/*------------------------------------------------------------------
 * Function:     Reverse
 * Purpose:      Reads through the list of integers and reverses the order.
 * Input args:   size = size of the array
 *				 list = original list
 * Return val:   void--none
 */
void Reverse(int size, int list[]) {
    int i, temp;
    int mid = size / 2;

    for (i = 0; i < mid; i ++){
    	temp = list[size - 1 - i];
    	list[size - 1 - i] = list[i];
    	list[i] = temp;
    }
} /*  Reverse  */

/*------------------------------------------------------------------
 * Function:     PrintList
 * Purpose:      Reads through the list of integers and prints it out.
 * Input args:   list = list of elements
 * Return val:   void--none
 */
void PrintList(int list[], int size) { 
    int i;

    for (i = 0; i < size; i ++){
    	printf("%d ", list[i]);
    }
printf("\n");
} /*  PrintList  */
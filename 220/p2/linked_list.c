/* File:     linked_list.c
 * Author:   Cayla Shaver
 * Section:  2
 *
 * Purpose:  Implement a sorted doubly linked list with ops insert (at sorted space),
 *           print, member, delete, free_list.
 * 
 * Input:    Single character lower case letters to indicate operators, 
 *           followed by arguments needed by operators.
 * Output:   Results of operations.
 *
 * Compile:  gcc -g -Wall -o linked_list linked_list.c
 * Run:      ./linked_list
 *
 * Notes:
 *    1.  Repeated values are not allowed in the list
 *    2.  delete only deletes the first occurrence of a value
 *    3.  Program assumes a string will be entered when prompted
 *        for one.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list_node_s {
   char   *string;
   struct list_node_s *prev_p;
   struct list_node_s *next_p;
};

struct list_s{
   struct list_node_s *head_p;
   struct list_node_s *tail_p;
};

const int MAX = 100;

int  Member(struct list_s *list, char *val);
void Insert(struct list_s *list, char *val);
void Delete(struct list_s *list, char *val);
void Print(struct list_s *list);
void Free_list(struct list_s *list); 
char Get_command(void);
void  Get_value(char *val);
struct list_node_s* Allocate(int size);
void Free_node(struct list_node_s *node_p);

/*-----------------------------------------------------------------*/
int main(void) {
   char string[MAX];
   char command;
   /* start with empty list */
   struct list_s list;
   list.head_p = NULL;
   list.tail_p = NULL;

   command = Get_command();
   while (command != 'q' && command != 'Q') {
      switch (command) {
         case 'i': 
         case 'I': 
            Get_value(string);
            Insert(&list, string);
            break;
         case 'p':
         case 'P':
            Print(&list);
            break;
         case 'm': 
         case 'M':
            Get_value(string);
            if (Member(&list, string))
               printf("%s is in the list\n", string);
            else
               printf("%s is not in the list\n", string);
            break;
         case 'd':
         case 'D':
            Get_value(string);
            Delete(&list, string);
            break;
         case 'f':
         case 'F':
            Free_list(&list);
            break;
         default:
            printf("There is no %c command\n", command);
            printf("Please try again\n");
      }
      command = Get_command();
   }

   Free_list(&list);

   return 0;
}  /* main */


/*-----------------------------------------------------------------
 * Function:    Member
 * Purpose:     search through the list for val
 * Input args:  *list: list structure that has been created
 *              val:  value to search for
 * Return val:  1 if val is in list, 0 otherwise
 */
int Member(struct list_s *list, char *val) {
   struct list_node_s* curr_p = list -> head_p;

   while (curr_p != NULL) {
      if (strcmp(curr_p -> string, val) == 0){
         return 1;
      }
      else{
         curr_p = curr_p -> next_p;
      }
   }
   return 0;
}  /* Member */


/*-----------------------------------------------------------------
 * Function:   Delete
 * Purpose:    Delete the first occurrence of val from list
 * Input args: *list: the list to go through to find the val
 *             val:    value to be deleted
 */
void Delete(struct list_s *list, char *val) {
   struct list_node_s *curr_p = list -> head_p;

   while (curr_p != NULL){
         if (strcmp(curr_p -> string, val) == 0){ 
           break;
         }
         else{
            curr_p = curr_p -> next_p;
         }
   }
   if (curr_p == NULL){
      printf("%s is either not in the list or the list is empty \n", val);
   }
   else if(curr_p == list -> head_p){
      if(curr_p -> next_p == NULL){
         Free_node(curr_p);
         list -> head_p = list -> tail_p = NULL;
      }
      else{
         curr_p -> next_p -> prev_p = curr_p -> prev_p;
         list->head_p = curr_p -> next_p;
         Free_node(curr_p);
      }
   }
   else if (curr_p == list -> tail_p){
         curr_p -> prev_p -> next_p = curr_p -> next_p;
         list->tail_p = curr_p -> prev_p;
         Free_node(curr_p);
   }
   else{ 
         curr_p -> prev_p -> next_p = curr_p -> next_p;
         curr_p -> next_p -> prev_p = curr_p -> prev_p;
         Free_node(curr_p);
   }
} /* Delete */


/*-----------------------------------------------------------------
 * Function:   Insert
 * Purpose:    Insert val into the proper alphabetical 
 *             space within the list
 * Input args: *list: the list to go through to add the val into
 *             val:  new value to be inserted
 */
void Insert(struct list_s *list, char *val) {
   struct list_node_s *curr_p = list -> head_p;
   struct list_node_s *new_node;

   new_node = Allocate(strlen(val) + 1);
   strncpy(new_node -> string, val, strlen(val) + 1);

   if(curr_p == NULL){
      list -> head_p = list -> tail_p = new_node;
      new_node -> prev_p = new_node -> next_p = NULL;
   }
   else{
      while(curr_p != NULL){
         if(strcmp(curr_p -> string, new_node -> string) >= 0){
            break;
         }
         curr_p = curr_p -> next_p;
      }
      if (curr_p == list -> head_p){
         new_node -> next_p = curr_p;
         new_node -> prev_p = curr_p -> prev_p;
         curr_p -> prev_p = new_node;
         list -> head_p = new_node;
      }
      else if (curr_p == NULL){
         new_node -> next_p =  list -> tail_p -> next_p;
         new_node -> prev_p = list -> tail_p;
         list -> tail_p -> next_p = new_node;
         list -> tail_p = new_node;
      }
      else{
         new_node -> next_p = curr_p;
         new_node -> prev_p = curr_p -> prev_p;
         curr_p -> prev_p -> next_p = new_node;
         curr_p -> prev_p = new_node;
      }
   }
}   /* Insert */

/*-----------------------------------------------------------------
 * Function:   Print
 * Purpose:    Print list on a single line of stdout
 * Input arg:  *list: list of nodes to print
 */
void Print(struct list_s *list) {
   struct list_node_s* curr_p = list -> head_p;

   printf("list = ");
   while (curr_p != NULL) {
      printf("%s ", curr_p -> string);
      curr_p = curr_p -> next_p;
   }  
   printf("\n");
}  /* Print */


/*-----------------------------------------------------------------
 * Function:    Free_list
 * Purpose:     free each node in the list, to relieve the storage space
 * Input arg:   *list: list of nodes that are to be "freed"
 * Note:        head_p and tail_p are set to NULL on completion, indicating
 *              list is empty.
 */
void Free_list(struct list_s *list) {
   struct list_node_s* curr_p;
   struct list_node_s* temp_p;

   curr_p = list -> head_p;
   while (curr_p != NULL) {
      temp_p = curr_p;
      curr_p = curr_p -> next_p;
      Free_node(temp_p);     
   }

   list -> head_p = list -> tail_p = NULL;
}  /* Free_list */

/*-----------------------------------------------------------------
 * Function:      Get_command
 * Purpose:       Get a single character command from stdin
 * Return value:  the first non-whitespace character from stdin
 */
char Get_command(void) {
   char c;

   printf("Please enter a command (i, p, m, d, f, q):  ");
   /* Put the space before the %c so scanf will skip white space */
   scanf(" %c", &c);
   return c;
}  /* Get_command */

/*-----------------------------------------------------------------
 * Function:   Get_value
 * Purpose:    Get a word from stdin
 * Note:       Behavior unpredictable if an word isn't entered
 */
void Get_value(char *val) {

   printf("Please enter a word:  ");
   scanf("%s", val);
}  /* Get_value */

/*-----------------------------------------------------------------
 * Function:    Allocate
 * Purpose:     Create the exact amount of space necessary 
 *              for the new word.
 * Input arg:   size: size of word to insert and the null terminator 
 *              after the word
 * Return val:  temp_p: the node with the exact amount of storage
 *              needed to store the word
 */
struct list_node_s* Allocate(int size) {
   struct list_node_s *temp_p;

   temp_p = malloc(sizeof(struct list_node_s));
   temp_p -> string = malloc(size * sizeof(char));
   
   return temp_p;
}  /* Allocate */

/*-----------------------------------------------------------------
 * Function:    Free_node
 * Purpose:     Free storage for the string then free the storage
 *              for the node to free up memory space
 * Input arg:   node_p: the node that will be removed
 */
void Free_node(struct list_node_s *node_p){
   
   free(node_p -> string);
   free(node_p);

}  /* Free_node */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list-factorial.h"

    //simple unit tests on the list
static void testList (void);
void showList (list l);

int main (int argc, char *argv[]) {

  testList ();
  printf ("All tests passed ! You are awesome\n");
  return EXIT_SUCCESS;
}

void testList (void){
  
  printf("Factorial 1\n");
  list fac1 = factorial(1);
  showList(fac1);
  list fac6 = factorial(6);
  showList(fac6);

}

void
showList (list l)
{

//  assert (l != NULL);
  if(l == NULL) return;
  // start at the first node
  link current = l->head;
  while (current != NULL)
    {
      printf ("[%d] -> ", current->value);
      current = current->next;
    }
  printf ("X\n");
}

// selectEven.c
// 
     
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
     
#include "list-factorial.h"


//return a list with 
//1 to x factorial
//factorial(3) would return a list with
// 1->2->6->X
//For x <= 0 just return an empty list
static int factorialCalc(int x);
static list newList(void);

list factorial (int x) {

    list l = newList();
    link curr = NULL;
    link nextNode;
    int index = 1;

    while(index <= x) {
        if(index == 0) {
            curr = malloc(sizeof(struct _node));
            curr->value = factorialCalc(index);
            curr->next = NULL;
            l->head = curr;
        } else {
            nextNode = malloc(sizeof(struct _node));
            nextNode->value = factorialCalc(index);
            nextNode->next = NULL;
            curr->next = nextNode;
            curr = curr->next;
        }
        index++;
    }
    return l;
}

static int factorialCalc(int x) {
    while(x > 1) {
        x *= (x - 1);
    }
    return x;
}

static list newList(void) {
    list newL = malloc(sizeof(struct _list));
    newL->head = NULL;
    return newL;
}

/*
 *  reverseList.c
 *  2013s1 Practice Prac Exam #1
 *  UNSW comp1917
 *
 *  Created by INSERT YOUR NAME HERE
 *  Share freely CC-BY-3.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "reverseList.h"

list reverse (list inputList) {

    int numNodes = 0;
    int index = 0;
    list curr;
    list temp = NULL;
    list outputList = NULL;
    curr = inputList;

    while(curr != NULL) {
        numNodes++;
        curr = curr->rest;
    }
  //  if(curr != NULL) {
        curr = inputList;
        while(index < numNodes) {
            if(outputList == NULL) {
                outputList = curr;
                curr = curr->rest;
                outputList->rest = NULL;
            } else {
                temp = outputList;
                outputList = curr;
                curr = curr->rest;
                outputList->rest = temp;
            }
            index++;
        }
    //}


    return outputList;
}

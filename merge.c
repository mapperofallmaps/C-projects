/*
 *  merge.c
 *  2016s1 Prac Exam #2
 *  UNSW comp1917
 *
 *  Created by Daniel Lau
 *  Share freely
 *	4/06/16
 */

#include <stdio.h>
#include <stdlib.h>
#include "merge.h"

void merge(list listA, list listB, list mergeList) {

	//keeping track of where you are in each of the 3 lists
	link currA = listA->head;
	link currB = listB->head;
	link currC = NULL;

	//Case 1 - both listA and listB are empty
	if(currA == NULL && currB == NULL) {
		mergeList->head = NULL;
	}

	//Case 2 - listB is empty
	if(currA != NULL && currB == NULL) {
		mergeList->head = currA;
	}

	//Case 3 - listA is empty
	if(currA == NULL && currB != NULL) {
		mergeList->head = currB;
	}

	//Case 4 - listA and listB are not empty

	//Case for the head for mergeList
	if(currA != NULL && currB != NULL) {
		//if a < b or a = b (when a = b, a still comes first)
		if(currA->value <= currB->value) {
			mergeList->head = currA;
			currC = currA;
			currA = currA->next;
		//if b < a
		} else if(currB->value < currA->value) {
			mergeList->head = currB;
			currC = currB;
			currB = currB->next;
		}
		//currC is head of mergeList now
		//currA and currB are holding the relative positions in their relative lists
		//Case for the rest of the mergeList
		while(currA != NULL || currB != NULL) {
			//if a < b or a = b (when a = b, a still comes first)
			if(currA->value <= currB->value || currA != NULL && currB == NULL) {
			currC->next = currA;
			//node after currC is currA now
			currA = currA->next;
			//move currA to the next node in listA
			currC = currC->next;
			//move currC to the end of mergeList or where currA was just formed
			//if b < a
			} else if(currB->value < currA->value || currB != NULL && currA == NULL) {
			currC->next = currB;
			currB = currB->next;
			currC = currC->next;
			//same thing
			}
		}
	}
	listA->head = NULL;
	listB->head = NULL;
	//making listA and listB empty and pointing to NULL
}
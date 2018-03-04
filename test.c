#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main (void) {


	int x;
	char str1[150];
	char str2[150];

	strcpy (str1, "R");

	x = strcmp (str1, "R");

	printf("%d", x);
}
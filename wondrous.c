/* 24/03/16
 * Daniel Lau
 * Wondrous Sequence
 */

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	return 0;
}

int printWondrous (int start) {
	int j = 1;

	scanf ("%d", &start);
	printf("%d ", start);

	while (start != 1) {
		if (start % 2 == 0) {
			start = start/2;
			printf ("%d ", start);
		} else {
			start = (start * 3) + 1;
			printf ("%d ", start);
		}
		j++;
	}
	return j;
}
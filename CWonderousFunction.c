/* 24/03/16
 * Daniel Lau
 * Wondrous Sequence
 */

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	int i;

	scanf ("%d", &i);
	printf("%d", i);

	while (i != 1) {
		if (i % 2) {
			i = i/2;
			printf ("%d ", i);
		} else {
			i = (i * 3) + 1;
			printf ("%d ", i);
		}
	}
	return EXIT_SUCCESS;
}
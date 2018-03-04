/* 24/03/16
 * Daniel Lau
 * Show Danish
 */

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	return 0;
}

void showDanish (void) {
	int i = 0;	

	while (i <= 4) {
		printf ("** *********\n");
		i++;
		while (i == 2) {
			printf ("\n");
			i++;
		}
	}
}
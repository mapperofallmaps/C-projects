/* 16/03/16
*  Daniel lau
*  Sort 3 Numbers
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

	int first, second, third;
	scanf ("%d %d %d", &first, &second, &third);

	if (first < second && first < third) {
		printf ("%d\n", first);
		if(second < third) {
			printf ("%d\n%d\n", second, third);
		} else {
			printf ("%d\n%d\n", third, second);
		}
	} else if (second < first && second < third) {
		printf ("%d\n", second);
		if(first < third) {
			printf ("%d\n%d\n", first, third);
		} else {
			printf ("%d\n%d\n", third, first);
		}
	} else if (third < first && third < second) {
		printf ("%d\n", third);
		if(first < second) {
			printf ("%d\n%d\n", first, second);
		} else {
			printf("%d\n%d\n", second, first);
		}
	}
	return EXIT_SUCCESS;
}

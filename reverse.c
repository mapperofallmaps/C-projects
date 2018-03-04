#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *reverse (char *message);

int main (int argc, char *argv[]) {

	char *string = "This is a string";

	char *reversestring;

	printf("%s\n", string);
	reversestring = reverse(string);
	printf("%s\n", reversestring);

	free (reversestring);

	return EXIT_SUCCESS;
}

char *reverse (char *message) {

	char *reversestring;

	reversestring = malloc(sizeof(char) * (strlen(message) + 1));

	int i = 0;
	int j = strlen(message) - 1;
	
	while (i<=j) {
		reversestring[i] = message[j-i];
		i++;
	}

	reversestring[j+1] = '\0';
	
	return reversestring;
}
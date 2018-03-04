#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

int main (int argc, char *argv[]) {

	char *message = "3.14";

    int size = strlen(message);
    int counter = 0;
    int sign = 1;
    int multiplier = 1;
    double number = 0;
    double number2 = 0;
    char ch;
    
    
    if (message[counter] == '-') {
            sign = -1;
            counter++;
    }

    while ((counter < size) && (message[counter] != '.')) {
        ch = message[counter];
        number = number * 10 + (ch - '0');
        counter++;
    }

    if (message[counter] == '.') {
        counter++;
    }

    while (message[counter] != '_') {
        ch = message[counter];
        number2 = number2 + ((ch - '0')/(multiplier * 10));
        counter++;
    }

    number = number + number2;

    if (sign == -1) {
        number = number * sign;
    }

    printf("%lf\n%lf\n", number, number2);

    return 0;
}
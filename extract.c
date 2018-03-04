// extract.c
// funtions and types used to extract x,y,z values from a
// string containing a url of the form
// "http://almondbread.cse.unsw.edu.au:7191/tile_x3.14_y-0.141_z5.bmp"
// initially by richard buckland
// 13 April 2014
// Modified by: Mark Joya, z5104126
// Date: 24/04/2016
// Tutor: William Archinal, Thu12Kora
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "extract.h"

#define DEC_BASE 10
#define ERROR_MARGIN 0.000001

void testmyAtoL (void);
void testmyAtoD (void);
 
 
int main (int argc, char *argv[]) {
    
    testmyAtoL();
    testmyAtoD();
    
    /*char * message = "http://almondbread.cse.unsw.edu.au:7191/tile_x3.14_y-0.141_z5.bmp";
 
    triordinate dat = extract (message);
 
    printf ("dat is (%f, %f, %d)\n", dat.x, dat.y, dat.z);
 
    assert (dat.x == 3.14);
    assert (dat.y == -0.141);
    assert (dat.z == 5);
    */
    
    //free(xString);
    //free(yString);
    //free(zString);
    
    return EXIT_SUCCESS;
}

triordinate extract (char* message) {
    triordinate points;
    
    int count = 0;
    int stringCounter = 0;
    
    char*xString = malloc(strlen(message));
    char*yString = malloc(strlen(message));    
    char*zString = malloc(strlen(message));    
    
    while (message[stringCounter] != 'x') {   
        stringCounter++;
    }
    stringCounter++;
    while (message[stringCounter] != '_') {
        xString[count] = message[stringCounter];
        count++;
        stringCounter++;
    }          
    
    xString[count] = '\0';
    
    count = 0;
    stringCounter = 0;
    
    while (message[stringCounter] != 'y') {   
        stringCounter++;
    }
    stringCounter++;
    while (message[stringCounter] != '_') {
        yString[count] = message[stringCounter];
        count++;
        stringCounter++;
    }          
 
    yString[count] = '\0';


    count = 0;
    stringCounter = 0;
    
    while (message[stringCounter] != 'z') {   
        stringCounter++;
    }
    stringCounter++;
    while (message[stringCounter] != '.') {
        zString[count] = message[stringCounter];
        count++;
        stringCounter++;
    }          
 
    zString[count] = '\0';
    
    points.x = myAtoD(xString);
    points.y = myAtoD(yString);
    points.z = myAtoL(zString);
    
    return points;

}

long myAtoL (char* message) {

    char* checkString = "0123456789";
    int counter = 0;
    int stringCounter = 0;
    long int result = 0;
    int size = strlen(message);
    int signMultiplier = 1;  
  
    if (message[0] == '-') {
        signMultiplier = -1;
        counter++;
    }  
  
    while (counter < size) {
        stringCounter = 0;
        while(message[counter] != checkString[stringCounter]) {
            stringCounter++;
        }   
        result += stringCounter * pow(DEC_BASE, size - counter - 1);
        counter++;
    }
    
    result *= signMultiplier;
    return result;
}

void testmyAtoL (void) {
    
    assert(myAtoL("10") == 10);
    assert(myAtoL("9") == 9);
    assert(myAtoL("0") == 0);
    assert(myAtoL("-5") == -5);
    
    printf("Function: myAtoL | All tests passed!\n");
    
    return;
}

//Use two counters to separate functionality?

double myAtoD (char *message) {

    int size = strlen(message);
    int counter = 0;
    int sign = 1;
    double multiplier = 1;
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

    while (counter < size) {
        ch = message[counter];
        number2 = number2 + ((ch - '0')/(multiplier * 10));
        multiplier = multiplier * 10;
        counter++;
    }
    
    number = number + number2;

    if (sign == -1) {
        number = number * sign;
    }

    return number;


  /*  char* checkString = "0123456789";
    
    int stringCounter = 0;
    int counter = 0;
    
    double result = 0;
    
    int size = strlen(message);
    int signMultiplier = 1;
    int intPart = 0;
    int decPlace = 0;
    
    while (counter < size) {
        if (message[counter] == '.') {
            intPart = counter;
        }
        counter++;
    }
    
    printf("Int part is %d\n", intPart);
    counter = 0;
    
    while (counter < size) {
        stringCounter = 0;
        if (message[counter] == '-') {
            signMultiplier = -1;
        } else {
            if (message[counter] == '.') {
                decPlace = counter;
            }             
            while ((message[counter] != checkString[stringCounter]) ||
                   (stringCounter != strlen(checkString))) {
                stringCounter++;
            }
            printf("Counter is: %d\n", counter);
            result += stringCounter * pow(DEC_BASE, intPart - counter - 1);
            printf("pow is: %lf\n", pow(DEC_BASE, intPart - counter - 1));
            printf("result is: %lf\n", result);
        }
        counter++;
    }
    
    result -= 9 * pow(DEC_BASE, intPart - decPlace - 1);
    
    result *= signMultiplier;
    return result;
*/
}

void testmyAtoD (void) {

    printf("3.14 is %lf\n", myAtoD("3.14"));

    assert(fabs(myAtoD("3.14") - 3.14) < ERROR_MARGIN);
    assert(fabs(myAtoD("15.147") - 15.147) < ERROR_MARGIN);    
    assert(fabs(myAtoD("0.0005") - 0.0005) < ERROR_MARGIN);
    assert(fabs(myAtoD("-1.50") - -1.5) < ERROR_MARGIN);
    assert(fabs(myAtoD("0") - 0) < ERROR_MARGIN);
    assert(fabs(myAtoD("5") - 5) < ERROR_MARGIN);
    assert(fabs(myAtoD("-3") - -3) < ERROR_MARGIN);    
    
    printf("Function: myAtoD | All tests passed!\n");
    
    return;
}

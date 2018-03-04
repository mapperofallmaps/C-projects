//Names: Mark Joya (z5104126), Daniel Lau (z3376221)
//Tutor: William Archinal, Thu12Kora
//Date: 19/04/2016
// Purpose: Defines the color scheme for each color of mandelbrot 

#include "pixelColor.h"

unsigned char stepsToRed (int steps) {
    int intensity = ((255 * steps) / 100);
    return intensity;
}

unsigned char stepsToBlue (int steps) {
    int intensity = ((255 * (100 - steps)) / 100);
    return intensity;
}

unsigned char stepsToGreen (int steps) {
    int intensity = steps;
    return intensity;
}



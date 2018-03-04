#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main (int argc, char *argv[]) {

//z=zx+zy*i
	double i;
	double x, x2;
	double y, y2;
	double Iteration = 0;
	double MaxIteration = 255;

	scanf("%d", &i);

	while(((x2 + y2) < 4) && Iteration < MaxIteration) {
		x2 = x * x;
		y2 = y * y * -1;
		Interation++;
	}	

	return Interation;
}
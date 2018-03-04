#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int escapeSteps (double x, double y) {

//z=zx+zy*i

	double xr = 0, xi = 0;
	double xr2 = 0, xi2 = 0;
	int iMax = 256;
	int i = 0;

// (a+bi)^2 = a^2 - b^2 + 2abi

	while(((xr * xr + xi * xi) < 4) && (i < iMax)) {
		xr2 = xr * xr;
		xi2 = xi * xi;
		
		xi = 2 * xr * xi + y;
		xr = xr2 - xi2 + x;
		i++;
	}

	return i;
}
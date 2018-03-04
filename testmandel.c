#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main (int argc, char *argv[]) {

//z=zx+zy*i
	double xr = 0, xi = 0;
	//double ore = 0, oim = 0;
	double xxr = 0;
	int iMax = 256;
	int i = 1;

	scanf("%lf %lf", &xr, &xi);

// (a+bi)^2 = a^2 - b^2 + 2abi

	while(((xr * xr + xi * xi) < 4) && (i < iMax)) {
		//ore = zre;
		//oim = zim;
		xxr = xr * xr - xi * xi;
		xi = 2 * xr * xi;
		xr = xxr;
		i++;
	}

	printf("%d\n", i);

	return 0;
}
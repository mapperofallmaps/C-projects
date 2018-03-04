#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	double x;
	double y;
	int z;
} point;

static int extract(char request[])
{
   char string[1000];

   strcpy( string, request );
   sscanf( string, "http://localhost:1916/tile_x%lf_y%lf_z%lf.bmp", &x, &y, &z );

   printf("%lf, %lf, %lf\n", x, y, z );

   point xyz = {x, y, z};
   
   return(xyz);
}

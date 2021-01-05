#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <math.h>
//#include<sys/types.h>
#include "xmmrand.c"
#include "logmmap.c"


#define N (unsigned int)1E9


int main(){
	uint64_t i;
	workspace_t w;
	double y, z;
	uint64_t pi=0;
	pbar barra;
	char* filename="reso.log";

	SetRandomSeed(&w);
	barra.filename=filename;
	barra.len=20;
	barra.err=0;
	barra.perc=0;
	barra.i=0;
	barra.Num=N;
	pbar_init(&barra);
	if (barra.err!=0){
		return -1;
	}

	for(i=0;i<N;i++){
		y = RSUnif(&w, 0., 1.);
		z = RSUnif(&w, 0., 1.);
		if((y*y + z*z) < 1)	pi++;

		//barra.i=i;
		//pbar_draw(&barra);
		if(i*100/(N-1)==barra.perc+1){
			//printf("perc= %lu\n", i*100/N);
			//barra.perc++;
			barra.i=i;
			pbar_draw(&barra);
		}
	}
	printf("pi = %.16le\n", (double)pi*4/N);

	printf("err=%u\n", barra.err);

	pbar_close(&barra);

	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/mman.h>
#include "xmmrand.c"
#include "logmmap.c"


#define N 1E10


int main(){
	uint64_t i;
	//uint64_t* s;
	workspace_t w;
	//vsd x;
	double y, z;
	uint64_t pi=0;
	pbar barra;

	//s=malloc(2*sizeof(uint64_t));
	//s[0] = 0x234654789876afdc;
	//s[1] = 0x13554368a987fdbb;
	//w.s1 = 0x234654789876afdc;
	//w.s2 = 0x13554368a987fdbb;
	//w.s3 = 0x2347e47f98e6afdc;
	//w.s4 = 0xba5c43c8a9d7fdbb;
	//w.state=0;
	
	SetRandomSeed(&w);
	barra=pbar_init();
	if (barra.err!=0)
		return -1;

	for(i=0;i<N;i++){
		y = RSUnif(&w, 0., 1.);
		z = RSUnif(&w, 0., 1.);
		//x = xorshift128v(&s);
		//printf("%.16le\n\t%.16le\n", y, z);
		//if((x[0]*x[0] + x[1]*x[1]) < 1)	pi++;
		if((y*y + z*z) < 1)	pi++;

		if(i*100/N==barra.perc+1){
			barra.perc++;
			pbar_draw(&barra);
		}
	}
	printf("pi = %.16le\n", (double)pi*4/N);

	pbar_close(&barra);

	return 0;

//	s[0] = 0x234654789876afdc;
//	s[1] = 0x13554368a987fdbb;
//	pi=0;
//        for(i=0;i<N;i++){
//                x = xorshift128(&s);
//		y = xorshift128(&s);
//		if((x*x + y*y) < 1)     pi++;
//                //printf("%.16le\n", x);
//        }
//	printf("pi = %.16le\n", (double)pi*4/N);
//
//	return 0;
}

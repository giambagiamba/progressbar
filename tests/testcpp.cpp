#include <iostream>
#include <cstdlib>

#define OPENMP_MODE
#ifdef OPENMP_MODE
  #include<omp.h>
#endif

#include "lib/xmmrand.c"
#include "../lib/logmmap.c"

#define N (uint64_t)1E9
#define NTHREADS 2

using namespace std;

int main(){
	uint64_t i;
	workspace_t w[NTHREADS];
	double y, z;
	uint64_t pi=0;
	pbar barra;
	char* filename=(char*)"reso.log";
	
	for(i=0;i<NTHREADS;i++){
		SetRandomSeed(w+i);
	}
	pbar_init(&barra, filename, N, 20, NTHREADS, '|');
	if (barra.err!=0){
		return -1;
	}
	#ifdef OPENMP_MODE
 	#pragma omp parallel for num_threads(NTHREADS) private(i, y, z) reduction(+:pi)
	#endif
	for(i=0;i<N;i++){
		#ifdef OPENMP_MODE
		workspace_t* W=w+omp_get_thread_num();
		#else
		workspace_t* W=w;
		#endif
		y = RSUnif(W, 0., 1.);
		z = RSUnif(W, 0., 1.);
		if((y*y + z*z) < 1){
			pi++;
		}
		//if(i*100/(N-1)==barra.perc+1){
			pbar_draw(&barra, i);
		//}
	}
	cout << "pi = " << (double)pi*4/N << endl;

	cout << "err= " << barra.err << endl;

	pbar_close(&barra);

	return 0;
}


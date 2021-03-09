#include <iostream>
#include <cstdlib>

#include "xmmrand.c"
#include "logmmap.c"

#define N (uint64_t)1E9

using namespace std;

int main(){
	uint64_t i;
	workspace_t w;
	double y, z;
	uint64_t pi=0;
	pbar barra;
	char* filename=(char*)"reso.log";

	SetRandomSeed(&w);
	pbar_init(&barra, filename, N, 20, 1, '|');
	if (barra.err!=0){
		return -1;
	}
	for(i=0;i<N;i++){
		y = RSUnif(&w, 0., 1.);
		z = RSUnif(&w, 0., 1.);
		
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


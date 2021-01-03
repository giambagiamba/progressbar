#include <stdint.h>
#include <math.h>

typedef struct{
        uint64_t s1, s2, s3, s4; //x - y
        uint64_t num;
        uint8_t state;
}workspace_t;

typedef double vsd __attribute__ ((vector_size(16)));
typedef uint64_t vsu __attribute__ ((vector_size(16)));

//Compilami con
//gcc -Wall -pedantic -c xmmrand.c -Ofast -march=native

void SetRandomSeed(workspace_t* w){
	uint32_t t;
	uint64_t v;
	
	asm volatile(
		"rdtsc\n"
		:"=a" (t)
		:
		:"%rdx"
	);
	v=(uint64_t)t;
	t=(t<<4)|(t>>28);
	v|=(uint64_t)t<<32;
	w->s1=v;

	t=(t<<4)|(t>>28);
	v=(uint64_t)t;
	t=(t<<4)|(t>>28);
	v|=(uint64_t)t<<32;
	w->s2=v;

	t=(t<<4)|(t>>28);
	v=(uint64_t)t;
	t=(t<<4)|(t>>28);
	v|=(uint64_t)t<<32;
        w->s3=v;
	
	t=(t<<4)|(t>>28);
        v=(uint64_t)t;
        t=(t<<4)|(t>>28);
        v|=(uint64_t)t<<32;
        w->s4=v;

	w->state=0;

	return;	
}


double RSUnif(workspace_t* w, double a, double b){
	vsu x, y, pres;
	double ret;

	if(w->state!=0){
		ret=((double)w->num)*pow(2,-63);
		w->state=~w->state;
		return ret*(b-a)+a;
	}

	x[0]=w->s1;
	x[1]=w->s2;
	y[0]=w->s3;
	y[1]=w->s4;

	w->s3=x[0];
	w->s4=x[1];

	x^=x<<23;
	pres=x^y^(x>>17)^(y>>26);
	w->s1=pres[0];
	w->s2=pres[1];

	pres=(pres+y)>>1;
	w->num=pres[1];
	ret=((double)pres[0])*pow(2,-63);

	w->state=~w->state;

	return ret*(b-a)+a;

}


/*
double xorshift128p(uint64_t** s){
        uint64_t x=**s;
        uint64_t const y=*(*s+1);
        **s=y;
        x^=x<<23;
        *(*s+1)=x^y^(x>>17)^(y>>26);

        return (double)((*(*s+1) + y)>>1)*exp(-63*log(2));
}

vsd xorshift128v(uint64_t** s){
        vsu* const xp = (vsu*)*s;
        vsu* const yp = xp+1;
        vsu x;
        vsu const y = *yp;
        vsu pres;
        vsd res;
        int64_t *d;

        x = *xp;

        *xp=y;
        x^= x<<23;
        *yp = x^y^(x>>17)^(y>>26);
        pres = (*yp+y)>>1;
        d = (int64_t*)&pres;

        res[0]=(double)d[0]*exp(-63*log(2));
        res[1]=(double)d[1]*exp(-63*log(2));

	return res;
}
*/


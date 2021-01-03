#include <stdio.h>
#include <stdlib.h>
//#include<sys/types.h>
//#include<sys/mman.h>

typedef struct{
	char* bar;
	unsigned int len;
	unsigned int perc;
	unsigned int nblks;
}pbar;

//Crea la struttura grafica della barra.
//La lunghezza e il puntatore dentro lo struct devono essere
//gia' preparati prima con mmap.
//Prevede barra lunga len e percentuale (xx%) e accapo a terminare.
int pbar_init(pbar* progbar){
	unsigned int i, len=progbar->len, perc=progbar->perc, nblks;
	char* mem=progbar->bar;

	if(perc<0 || perc>99)	return -1;
	if(mem==0)	return -1;
	if(len<3)	return -1;

	nblks=perc*(len)/100;
	progbar->nblks=nblks;
	mem[0]='[';
	for(i=1;i<nblks+1;i++){
		mem[i]='|';
	}
	for(;i<len+1;i++){
		mem[i]=' ';
	}
	mem[len+1]=']';
	mem[len+2]=(perc/10)+48;
	mem[len+3]=(perc%10)+48;
	mem[len+4]='%';
	mem[len+5]=0xa;

	return 0;
}

int pbar_draw(pbar* progbar){
	char* mem = progbar->bar;
	unsigned int nblks, len=progbar->len, perc=progbar->perc;

	if(perc<0 || perc>99)   return -1;
	if(mem == 0) return -1;
	
	nblks=perc*len/100; //New number of blocks
	//progbar->bar=mem;
	if(nblks!=progbar->nblks){ //If new nblks > old one...
		progbar->nblks=nblks; //Update nblks
		mem[nblks]='|'; //Draw another block
	}
	mem[len+2]=(perc/10)+48;
	mem[len+3]=(perc%10)+48;

	return 0;
}

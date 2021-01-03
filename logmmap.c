#include <stdio.h>
#include <stdlib.h>
//#include<sys/types.h>
//#include<sys/mman.h>

typedef struct{
	char* bar;
	unsigned int len;
	unsigned int perc;
	unsigned int nblks;
	int err;
	void* A;
	int file;
}pbar;

//Crea la struttura grafica della barra.
//La lunghezza e il puntatore dentro lo struct devono essere
//gia' preparati prima con mmap.
//Prevede barra lunga len e percentuale (xx%) e accapo a terminare.
pbar pbar_init(){
	unsigned int i, len, perc, nblks;
	char* mem;
	pbar progbar;
	
	progbar.err=0;

	progbar.file=open("reso.log", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
	if(progbar.file<0){
		printf("file: %x\n", progbar.file);
		progbar.err=-1;
	}

	if(posix_fallocate(progbar.file, 0, 32)!=0){
		printf("Errore fallocate\n");
		progbar.err=-1;
	}

	progbar.A=mmap(0, 32, PROT_WRITE|PROT_READ, MAP_SHARED, progbar.file, 0);
	progbar.len=20;
	progbar.bar=(char*)progbar.A;
	progbar.perc=0;
	
	len=progbar.len;
	perc=progbar.perc;
	mem=progbar.bar;

	if(perc<0 || perc>99){
		progbar.err=-1;
		return progbar;
	}
	if(mem==0){
		progbar.err=-1;
		return progbar;
	}
	if(len<3){
		progbar.err=-1;	
		return progbar;
	}
	
	nblks=perc*(len)/100;
	progbar.nblks=nblks;
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
	
	printf("barra: %p %u %u\n", progbar.bar, progbar.len, progbar.perc);

	return progbar;
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

void pbar_close(pbar* progbar){
	munmap(progbar->A, 32);
	close(progbar->file);
}

#include <stdio.h>
#include <stdlib.h>
//#include<sys/types.h>
//#include<sys/mman.h>

#define ERR_FILEOPEN 1
#define ERR_FILEALLOC 2
#define ERR_MMAP 4
#define WARN_PERC 8
#define WARN_LEN 16
#define ERR_MEM 32


typedef struct{
	char* bar;
	unsigned int len;
	unsigned int perc;
	unsigned int nblks;
	unsigned int err;
	char* filename;
	int file;
}pbar;

//Crea la struttura grafica della barra.
//La lunghezza e il puntatore dentro lo struct devono essere
//gia' preparati prima con mmap.
//Prevede barra lunga len e percentuale (xx%) e accapo a terminare.
void pbar_init(pbar* progbar){
	unsigned int i, len, perc, nblks;
	char* mem;
	int file;
	
	progbar->err=0;
	len=progbar->len;
	perc=progbar->perc;

	file=open(progbar->filename, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
	if(file<0){
		//printf("file: %x\n", progbar.file);
		progbar->err|=ERR_FILEOPEN;
		return;
	}

	if(posix_fallocate(file, 0, len+7)!=0){
		//printf("Errore fallocate\n");
		progbar->err|=ERR_FILEALLOC;
		return;
	}

	progbar->file=file;
	mem=mmap(0, len+7, PROT_WRITE|PROT_READ, MAP_SHARED, file, 0);
	if((void*)mem==(void*)-1){
		progbar->err|=ERR_MMAP;
		return;
	}
	//progbar.len=20;
	//progbar.bar=(char*)progbar.A;
	//progbar.perc=0;
	
	progbar->bar=mem;

	if(perc<0 || perc>100){
		progbar->err|=WARN_PERC;
		perc=0;
		//return;
	}
	if(len<3){
		progbar->err|=WARN_LEN;
		len=8;
		//return;
	}
	
	nblks=perc*len/100;
	progbar->nblks=nblks;
	mem[0]='[';
	for(i=1;i<nblks+1;i++){
		mem[i]='|';
	}
	for(;i<len+1;i++){
		mem[i]=' ';
	}
	mem[len+1]=']';
	mem[len+2]=(perc/100)+'0';
	mem[len+3]=((perc/10)%10)+'0';
	mem[len+4]=(perc%10)+'0';
	mem[len+5]='%';
	mem[len+6]=0xa;
	
	//printf("barra: %p %u %u\n", progbar.bar, progbar.len, progbar.perc);

	return;
}

void pbar_draw(pbar* progbar){
	char* mem = progbar->bar;
	unsigned int nblks, len=progbar->len, perc=progbar->perc;

	if(mem == 0){
		progbar->err|=ERR_MEM;
		return;
	}
	if(perc<0 || perc>100){
	     	progbar->err|=WARN_PERC;
		perc=0;
	}
	
	nblks=perc*len/100; //New number of blocks
	//progbar->bar=mem;
	if(nblks==(progbar->nblks)+1){ //If new nblks == old one+1...
		progbar->nblks=nblks; //Update nblks
		mem[nblks]='|'; //Draw another block
	}
	else{
		//TODO
	}
	mem[len+2]=(perc/100)+'0';
	mem[len+3]=((perc/10)%10)+'0';
	mem[len+4]=(perc%10)+'0';

	return;
}

void pbar_close(pbar* progbar){
	munmap(progbar->bar, progbar->len+7);
	close(progbar->file);
}

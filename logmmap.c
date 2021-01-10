#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<sys/mman.h>
#define ADDLEN 23

#define ERR_FILEOPEN 1
#define ERR_FILEALLOC 2
#define ERR_MMAP 4
#define WARN_PERC 8
#define WARN_LEN 16
#define ERR_MEM 32
#define WARN_NUM 64
#define WARN_NT 128

//#ifdef OPENMP_MODE
//	#define MAX (progbar->Num/progbar->NT)
//	#define I (argi%MAX)
//#else
//	#define MAX (progbar->Num)
//	#define I (argi) //For efficiency
//#endif


typedef struct{
	char* bar;
	unsigned int len;
	//unsigned int perc;
	//uint64_t i;
	uint64_t Num;
	uint64_t max;
	unsigned int perc;
	unsigned int nblks;
	unsigned int err;
	//char* filename;
	int file;
	double start;
	unsigned int NT;
}pbar;


void pbar_init(pbar* progbar, char* filename, uint64_t Num, unsigned int len, unsigned int NT){
	uint64_t i, iNum;
	unsigned int ilen, iNT;
	char* mem;
	int file;
	
	//progbar->i=0;
	progbar->err=0;
	
	ilen=len;
	iNum=Num;
	iNT=NT;
	if(len<3){
		progbar->err|=WARN_LEN;
		ilen=8;
	}
	if(Num==0){
		progbar->err|=WARN_NUM;
		iNum=1e6;
	}
	if(NT>Num){
		progbar->err|=WARN_NT;
		iNT=1;
	}
	progbar->Num=iNum;
	progbar->max=iNum/iNT;
	progbar->len=ilen;
	progbar->NT=iNT;

	file=open(filename, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
	if(file<0){
		//printf("file: %x\n", progbar.file);
		progbar->err|=ERR_FILEOPEN;
		return;
	}

	if(posix_fallocate(file, 0, ilen+ADDLEN+1)!=0){
		//printf("Errore fallocate\n");
		progbar->err|=ERR_FILEALLOC;
		return;
	}

	progbar->file=file;
	mem=mmap(0, ilen+ADDLEN+1, PROT_WRITE|PROT_READ, MAP_SHARED, file, 0);
	if((void*)mem==(void*)-1){
		progbar->err|=ERR_MMAP;
		return;
	}
	
	progbar->bar=mem;
	//progbar->filename=filename;

	//nblks=i*ilen/(iNum-1);
	//perc=i*100/(iNum-1);
	progbar->nblks=0;
	progbar->perc=0;
	mem[0]='[';
	//for(i=1;i<nblks+1;i++){
	//	mem[i]='|';
	//}
	for(i=1;i<ilen+1;i++){
		mem[i]=' ';
	}
	mem[ilen+1]=']';
	
	mem[ilen+2]=' ';
	mem[ilen+3]=' ';
	mem[ilen+4]='0';
	
	mem[ilen+5]='%';
	//mem[ilen+6]=0xa;
	for(i=ilen+6;i<ilen+19;i++){
		mem[i]=' ';
	}
	mem[ilen+19]=' ';
	mem[ilen+20]='E';
	mem[ilen+21]='T';
	mem[ilen+22]='A';
	mem[ilen+23]=0xa;
	
	//printf("barra: %p %u %u\n", progbar.bar, progbar.len, progbar.perc);
	
	//progbar->start=time(NULL);
	{
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		progbar->start=(double)ts.tv_sec+(double)ts.tv_nsec*1e-9;
	}

	return;
}

void pbar_eta(pbar* progbar, uint64_t argi)__attribute__((always_inline));
void pbar_eta(pbar* progbar, uint64_t argi){
	char* mem = progbar->bar;
	unsigned int len=progbar->len;
	double start=progbar->start;
	double now, est, diff;
	time_t et;
	struct tm* eta;
	
	{
		struct timespec tn;
		clock_gettime(CLOCK_MONOTONIC, &tn);
		now=(double)tn.tv_sec+(double)tn.tv_nsec*1e-9;
	}
	diff=now-start;
	est=(progbar->max)*diff/argi;
	est-=diff;
	et=(time_t)est;
	//fprintf(stderr, "Dif: %lf\n", diff);
	//fprintf(stderr, "Est: %lf\n", est);
	eta=localtime(&et);
	//fprintf(stderr, "%lu\n", eta->tm_hour);
	
	mem[len+6]=' ';
	mem[len+7]=eta->tm_yday/10+'0';
	mem[len+8]=eta->tm_yday%10+'0';
	mem[len+9]='d';
	mem[len+10]=' ';
	mem[len+11]=eta->tm_hour/10+'0';
	mem[len+12]=eta->tm_hour%10+'0'-1; //Absurd...
	mem[len+13]=':';
	mem[len+14]=eta->tm_min/10+'0';
	mem[len+15]=eta->tm_min%10+'0';
	mem[len+16]=':';
	mem[len+17]=eta->tm_sec/10+'0';
	mem[len+18]=eta->tm_sec%10+'0';

	return;
}

void pbar_draw(pbar* progbar, uint64_t argi){
	char* mem = progbar->bar;
	unsigned int nblks, perc, len=progbar->len, i;
	uint64_t vari=argi, max=progbar->max;
	unsigned int u, d, c;

#ifdef OPENMP_MODE
	if(argi>progbar->max)	return; //keep only first thread
#endif

	if(mem == 0){
		progbar->err|=ERR_MEM;
		return;
	}
	//vari=argi;
	//printf("%lu - %lu\n", vari, MAX);
#ifndef OPENMP_MODE
	if(vari>progbar->Num){
	     	progbar->err|=WARN_PERC;
		vari=0;
	}
#endif
	//progbar->i=vari;
	perc=vari*100/(max-1);
	
	//printf("Perc= %u\t%u\n", i, perc);
	if(perc==progbar->perc) return;//Same percentage, nothing to do
	else{
		//printf("\tqui\n");
		progbar->perc=perc;//Update stored perc
		nblks=vari*len/(max-1); //Calculate new number of blocks
                if(nblks==(progbar->nblks)+1){ //Add one block
			//printf("\t\tnewblock\n");
	       	        mem[nblks]='|'; //Draw another block
	        }
		else if(nblks!=progbar->nblks){//Not the same, redraw whole bar
			for(i=1;i<nblks;i++){
                	        mem[i]='|';
	                }
        	        for(;i<len+1;i++){
        	                mem[i]=' ';
	                }
		}
		progbar->nblks=nblks; //Update stored nblks
		u=perc%10;
		d=(perc/10)%10;
		c=perc/100;
		mem[len+2]=c*(1+'0'-' ')+' ';
		mem[len+3]=(c+d==0) ? ' ' : d+'0';//Migliorare
		mem[len+4]=u+'0';
	}
	
	pbar_eta(progbar, vari);
	
	return;
}

void pbar_close(pbar* progbar){
	munmap(progbar->bar, progbar->len+ADDLEN+1);
	close(progbar->file);
}

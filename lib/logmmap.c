 /* progressbar/logmmap.c
 * Stored and maintained at https://github.com/giambagiamba/progressbar
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
 
#ifndef LOGMMAP_C
#define LOGMMAP_C

 #ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

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


typedef struct{
	char* bar;
	char fill;
	unsigned int len;
	uint64_t max;
	unsigned int perc;
	unsigned int nblks;
	unsigned int err;
	int file;
	double start;
	unsigned int NT;
}pbar;


void pbar_init(pbar* progbar, char* filename, uint64_t Num, unsigned int len, unsigned int NT, char fill){
	uint64_t i, iNum;
	unsigned int ilen, iNT;
	char* mem;
	int file;
	
	progbar->err=0;
	progbar->fill=fill;
	
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
	#ifdef __cplusplus
	mem=static_cast<char*>(mmap(0, ilen+ADDLEN+1, PROT_WRITE|PROT_READ, MAP_SHARED, file, 0));
	#endif
	#ifndef __cplusplus
	mem=mmap(0, ilen+ADDLEN+1, PROT_WRITE|PROT_READ, MAP_SHARED, file, 0);
	#endif
	if((void*)mem==(void*)-1){
		progbar->err|=ERR_MMAP;
		return;
	}
	
	progbar->bar=mem;
	progbar->nblks=0;
	progbar->perc=0;
	mem[0]='[';
	for(i=1;i<ilen+1;i++){
		mem[i]=' ';
	}
	mem[ilen+1]=']';
	
	mem[ilen+2]=' ';
	mem[ilen+3]=' ';
	mem[ilen+4]='0';
	
	mem[ilen+5]='%';
	for(i=ilen+6;i<ilen+19;i++){
		mem[i]=' ';
	}
	mem[ilen+19]=' ';
	mem[ilen+20]='E';
	mem[ilen+21]='T';
	mem[ilen+22]='A';
	mem[ilen+23]=0xa;
	
	{
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		progbar->start=(double)ts.tv_sec+(double)ts.tv_nsec*1e-9;
	}

	return;
}

//void pbar_eta(pbar* progbar, uint64_t argi)__attribute__((always_inline));
void pbar_eta(char*mem, unsigned int len, uint64_t max, double start, uint64_t argi)__attribute__((always_inline));
void pbar_eta(char*mem, unsigned int len, uint64_t max, double start, uint64_t argi){
	//char* mem = progbar->bar;
	//unsigned int len=progbar->len, max=progbar->max;
	//double start=progbar->start;
	double now, est, diff;
	time_t et;
	struct tm* eta;
	
	{
		struct timespec tn;
		clock_gettime(CLOCK_MONOTONIC, &tn);
		now=(double)tn.tv_sec+(double)tn.tv_nsec*1e-9;
	}
	diff=now-start;
	est=(max-argi)*diff/argi;
	et=(time_t)est;
	eta=localtime(&et);
	
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
	char* mem;
	unsigned int nblks, perc, len, i;
	uint64_t vari, max;
	unsigned int u, d, c;

	vari=argi;
	if(vari>=progbar->max){
		#ifdef OPENMP_MODE	//If more thread this condition is true when
			return;		//calling thread is not the first one.
		#else
			progbar->err|=WARN_PERC; //If 1 thread i is over max
			vari=0; //Default to 0
		#endif
	}
	max=progbar->max;
	perc=vari*100/(max-1);
	//fprintf(stderr,"vari=%lu\n", vari);
	if(perc==progbar->perc) return;//Same percentage, nothing to do
	else{
		mem=progbar->bar;
		if(mem == 0){
			progbar->err|=ERR_MEM;
			return;
		}
		progbar->perc=perc;//Update stored perc
		len=progbar->len;
		nblks=vari*len/(max-1); //Calculate new number of blocks
                if(nblks==(progbar->nblks)+1){ //Add one block
	       	        mem[nblks]=progbar->fill; //Draw another block
	        }
		else if(nblks!=progbar->nblks){//Not the same, redraw whole bar
			for(i=1;i<nblks;i++){
                	        mem[i]=progbar->fill;
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
	//char*mem, unsigned int len, unsigned int max, double start, uint64_t argi
	pbar_eta(mem, len, max, progbar->start, vari);
	
	return;
}

/*Same as pbar_draw when multithreading option is active*/
void pbar_draw_mt(pbar* progbar, uint64_t argi){
	char* mem;
	unsigned int nblks, perc, len, i;
	uint64_t vari, max;
	unsigned int u, d, c;

	vari=argi;
	if(vari>=progbar->max){
			return;		//calling thread is not the first one.
	}
	max=progbar->max;
	perc=vari*100/(max-1);
	//fprintf(stderr,"vari=%lu\n", vari);
	if(perc==progbar->perc) return;//Same percentage, nothing to do
	else{
		mem=progbar->bar;
		if(mem == 0){
			progbar->err|=ERR_MEM;
			return;
		}
		progbar->perc=perc;//Update stored perc
		len=progbar->len;
		nblks=vari*len/(max-1); //Calculate new number of blocks
                if(nblks==(progbar->nblks)+1){ //Add one block
	       	        mem[nblks]=progbar->fill; //Draw another block
	        }
		else if(nblks!=progbar->nblks){//Not the same, redraw whole bar
			for(i=1;i<nblks;i++){
                	        mem[i]=progbar->fill;
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
	//char*mem, unsigned int len, unsigned int max, double start, uint64_t argi
	pbar_eta(mem, len, max, progbar->start, vari);
	
	return;
}

void pbar_close(pbar* progbar){
	munmap(progbar->bar, progbar->len+ADDLEN+1);
	close(progbar->file);
	//fprintf(stderr, "err_c=%u\n", progbar->err);
}

void p_init(pbar* progbar, char* filename, int64_t* Num, int64_t* len, int64_t* NT, char* fill){
	uint64_t N, l, N_T;
	char f;

	N=(uint64_t) *Num;
	l=(uint64_t) *len;
	N_T=(uint64_t) *NT;
	f=*fill;

	int i=0;
	while (filename[i]!=' '){
		i++;
	}
	filename[i]='\0';
	fprintf(stderr, "filename=%s\n", filename);
	pbar_init(progbar, filename, N, l, N_T, f);
	return;
}

void p_draw(pbar* progbar, int64_t* argi){
	uint64_t arg_i;
	arg_i=(uint64_t) *argi;
	pbar_draw(progbar, arg_i-1);
	return;
}

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*LOGMMAP_C*/

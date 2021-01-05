#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
//#include<sys/types.h>
#include<sys/mman.h>

#define ERR_FILEOPEN 1
#define ERR_FILEALLOC 2
#define ERR_MMAP 4
#define WARN_PERC 8
#define WARN_LEN 16
#define ERR_MEM 32


typedef struct{
	char* bar;
	unsigned int len;
	//unsigned int perc;
	unsigned int i;
	unsigned int Num;
	unsigned int perc;
	unsigned int nblks;
	unsigned int err;
	char* filename;
	int file;
	time_t start;
}pbar;

//Crea la struttura grafica della barra.
//La lunghezza e il puntatore dentro lo struct devono essere
//gia' preparati prima con mmap.
//Prevede barra lunga len e percentuale (xx%) e accapo a terminare.
void pbar_init(pbar* progbar){
	unsigned int i, Num, len, perc, nblks;
	unsigned int u, d, c;
	char* mem;
	int file;
	
	progbar->err=0;
	len=progbar->len;
	//perc=progbar->perc;

	file=open(progbar->filename, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
	if(file<0){
		//printf("file: %x\n", progbar.file);
		progbar->err|=ERR_FILEOPEN;
		return;
	}

	if(posix_fallocate(file, 0, len+24)!=0){
		//printf("Errore fallocate\n");
		progbar->err|=ERR_FILEALLOC;
		return;
	}

	progbar->file=file;
	mem=mmap(0, len+24, PROT_WRITE|PROT_READ, MAP_SHARED, file, 0);
	if((void*)mem==(void*)-1){
		progbar->err|=ERR_MMAP;
		return;
	}
	//progbar.len=20;
	//progbar.bar=(char*)progbar.A;
	//progbar.perc=0;
	
	progbar->bar=mem;

	i=progbar->i;
	Num=progbar->Num;
	if(i<0 || i>Num-1){
		progbar->err|=WARN_PERC;
		perc=0;
		//return;
	}
	if(len<3){
		progbar->err|=WARN_LEN;
		len=8;
		//return;
	}
	
	nblks=(long unsigned int)i*len/(Num-1);
	progbar->nblks=nblks;
	mem[0]='[';
	for(i=1;i<nblks+1;i++){
		mem[i]='|';
	}
	for(;i<len+1;i++){
		mem[i]=' ';
	}
	mem[len+1]=']';
	
	perc=(long unsigned int)i*100/(Num-1);
	progbar->perc=perc;
	u=perc%10;
	d=(perc/10)%10;
	c=perc/100;
	mem[len+2]=c*(1+'0'-' ')+' ';
	mem[len+3]=(c+d==0) ? ' ' : d+'0';//Migliorare
	mem[len+4]=u+'0';
	
	//mem[len+2]=c+'0';
	//mem[len+3]=((perc/10)%10)+'0';
	//mem[len+4]=u+'0';
	mem[len+5]='%';
	//mem[len+6]=0xa;
	mem[len+19]=' ';
	mem[len+20]='E';
	mem[len+21]='T';
	mem[len+22]='A';
	mem[len+23]=0xa;
	
	//printf("barra: %p %u %u\n", progbar.bar, progbar.len, progbar.perc);
	
	progbar->start=time(NULL);

	return;
}

void pbar_eta(pbar* progbar){
	char* mem = progbar->bar;
	unsigned int len=progbar->len;
	time_t start=progbar->start;
	time_t now, est, diff, et;
	struct tm* eta;
	
	now=time(NULL);
	diff=difftime(now, start);
	est=(time_t)progbar->Num/(progbar->i-1.)*diff;
	et=est-diff;
	//fprintf(stderr, "%lu\n", est);
	//est=(time_t) t*N;
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

void pbar_draw(pbar* progbar){
	char* mem = progbar->bar;
	unsigned int nblks, perc, len=progbar->len, i=progbar->i, Num=progbar->Num;
	unsigned int u, d, c;

	perc=(long unsigned int)i*100/(Num-1);//Intermediate long to avoid overflow

	if(mem == 0){
		progbar->err|=ERR_MEM;
		return;
	}
	if(i>(Num-1)){
	     	progbar->err|=WARN_PERC;
		perc=0;
	}
	
	//printf("Perc= %u\t%u\n", i, perc);
	if(perc==progbar->perc) return;//Same percentage, nothing to do
	else{
		//printf("\tqui\n");
		nblks=(long unsigned int)i*len/(Num-1); //Calculate new number of blocks
		progbar->perc=perc;//Update stored perc
		//if(nblks==progbar->nblks){//Same nblks, just update percentage
		//	u=perc%10;
                //        d=(perc/10)%10;
                //        c=perc/100;
                //        mem[len+2]=c*(1+'0'-' ')+' ';
                //        mem[len+3]=(c+d==0) ? ' ' : d+'0';//Migliorare
                //        mem[len+4]=u+'0';
                //        return;
		//}
                if(nblks==(progbar->nblks)+1){ //Add one block
			//printf("\t\tnewblock\n");
	       	        mem[nblks]='|'; //Draw another block
	        }
		else if(nblks!=progbar->nblks){//Not the same, redraw whole bar
			for(i=0;i<nblks;i++){
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
	
	pbar_eta(progbar);
	
	return;


//	if(nblks==progbar->nblks){//Ottimizzazione
//		if(perc==progbar->perc)	return;
//		else{
//			u=perc%10;
//		        d=(perc/10)%10;
//			c=perc/100;
//			mem[len+2]=c*(1+'0'-' ')+' ';
//			mem[len+3]=(c+d==0) ? ' ' : d+'0';//Migliorare
//			mem[len+4]=u+'0';
//			progbar->perc=perc;
//
//			return;
//		}
//	}
//	if(nblks==(progbar->nblks)+1){ //If new nblks == old one+1...
//		progbar->nblks=nblks; //Update nblks
//		mem[nblks]='|'; //Draw another block
//	}
//	else if(nblks!=progbar->nblks){//Else redraw whole bar
//		for(i=0;i<nblks;i++){
//			mem[i]='|';
//		}
//		for(;i<len+1;i++){
//			mem[i]=' ';
//		}
//	}
//	progbar->perc=perc;
//	u=perc%10;
//	d=(perc/10)%10;
//	c=perc/100;
//	mem[len+2]=c*(1+'0'-' ')+' ';
//	mem[len+3]=(c+d==0) ? ' ' : d+'0';//Migliorare
//	mem[len+4]=u+'0';
//
//	return;
}

void pbar_close(pbar* progbar){
	munmap(progbar->bar, progbar->len+23);
	close(progbar->file);
}

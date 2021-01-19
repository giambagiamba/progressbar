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


void pbar_init(pbar* progbar, char* filename, uint64_t Num, unsigned int len, unsigned int NT, char fill);

void pbar_eta(pbar* progbar, uint64_t argi)__attribute__((always_inline));

void pbar_draw(pbar* progbar, uint64_t argi);

void pbar_close(pbar* progbar);

void p_init(pbar* progbar, char* filename, int64_t* Num, int64_t* len, int64_t* NT, char* fill);

void p_draw(pbar* progbar, int64_t* argi);

 /* progressbar/logmmap.h
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

#ifndef LOGMMAP_H
#define LOGMMAP_H

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

#ifdef OPENMP_MODE
#define pbar_draw pbar_draw_mt
#else
#define pbar_draw pbar_draw
#endif /*OPENMP_MODE*/

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

void pbar_eta(char*mem, unsigned int len, uint64_t max, double start, uint64_t argi)__attribute__((always_inline));

void pbar_draw(pbar* progbar, uint64_t argi);

void pbar_draw_mt(pbar* progbar, uint64_t argi);

void pbar_close(pbar* progbar);

void p_init(pbar* progbar, char* filename, int64_t* Num, int64_t* len, int64_t* NT, char* fill);

void p_draw(pbar* progbar, int64_t* argi);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*LOGMMAP_H*/

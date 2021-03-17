# Progress bar
Simple C library for drawing a progress bar on a memory mapped log file. It is designed to introduce as little overhead as possible. It is compatible with C++ and Fortran.

## Table of Contents
1. [Content of the library](#content)
	1. [logmmap.c](#logmmap)
	2. [logmmapf.f90](#logmmapf)
2. [How to use](#howto)
	1. [C/C++](#C)
	2. [Fortran](#F90)
3. [Thread-safety](#Thread)
4. [Errors](#Errors)
5. [Examples](#Examples)
	
## Content of the library <a name="content"></a>
The files are stored in two directories: lib/ and tests/. Go to [Examples](#Examples) to see the content of the second folder.
### logmmap.c <a name="logmmap"></a>
C library (C++ compatible).
The pbar structured type is defined. It contains:

* char\* bar: A string with the content of the progress bar to be drawn;
* char fill: the char used to display the progress;
* unsigned int len: length of the bar, i.e. the number of blocks to be drawn at 100%;
* uint64\_t max: Number of iterations of the job related to the bar;
* unsigned int perc: percentage, set by caller;
* unsigned int nblks: number of full blocks, calculated internally from percentage;
* unsigned int err: error code;
* int file: log file where the bar is drawn;
* double start: initial time used for ETA calculation;
* unsigned int NT: number of threads.

Seven functions are available: pbar\_init, pbar\_eta, pbar\_draw, pbar\_draw\_mt, pbar\_close, p\_init, p\_draw. They are all void-returning.

* `void pbar_init(pbar* progbar, char* filename, uint64_t Num, unsigned int len, unsigned int NT, char fill)`: It opens filename and maps it in memory (using open, fallocate and mmap), and initializes the content of pbar according to its arguments. i, perc, err and nblks are set to 0.
* `void pbar_eta(pbar* progbar, uint64_t argi)`: It calculates and prints ETA. It is called by pbar\_draw.
* `void pbar_draw(pbar* progbar, uint64_t argi)`: It updates progbar-\>nblks and perc according to argi, and draw the bar, the new percentage and ETA. In case the new calculated percentage is the same stored in progbar, it returns. The same is done after checking the new calculated value of nblks.
* `void pbar_draw_mt(pbar* progbar, uint64_t argi)`: it does the same as pbar\_draw but also in multithreading even if called from loggmap.h. A macro in logmmap.h set pbar\_draw as alias for pbar\_draw\_mt.
* `pbar_close(pbar* progbar)`: It unmaps the file and closes it.
* `p_init(pbar* progbar, char* filename, int64_t* Num, int64_t* len, int64_t* NT, char* fill)` and `p_draw(pbar* progbar, int64_t* argi)`: wrapper. They are called by logmmapf.f90.

### logmmapf.f90 <a name="logmmapf"></a>
Fortran wrapper of the C library.
The module pbar\_type is defined. It is a wrapper of the pbar structured type.
The module logmmap defines the wrappers for the subroutines pbar\_init, pbar\_draw, pbar\_close. 

## How to use <a name="howto"></a>
### C/C++ <a name="C"></a>
* Include directly "logmmap.c" in order to have a better performance. The file has to be included AFTER the define of PROGRESSBAR\_MT.
* Declare a variable of type pbar.
* Call pbar\_init passing as first argument the pointer to the pbar variable and the others according to your choice. In case of error, pbar.err will be set (see [Errors](#Errors) section).
* In your job cycle, update the bar by calling pbar\_draw. It is suggested to avoid to do the call every cycle, instead check that the calculated percentage is different from the previous value. Calling pbar\_draw with the same value of percentage won't change the bar but introduces the overhead of the call uselessly.
* When done, call pbar\_close to unmap the file and close it.
* The bar is stored in a file. In order to visualize the bar is suggested to use, under UNIX systems, `watch cat <file>`.

### Fortran <a name="F2003"></a>
* Fortran2003 or later is required due to `iso_c_binding`.
* Use `pbar_type` and `logmmap` (stored in file logmmapf.f90).
* Declare a variable of `type(pbar_t)`.
* Follow the same steps as in C/C++.

## Thread-safety <a name="Thread"></a>
Tested in C and C++ using OpenMP.

## Errors <a name="Errors"></a>
Errors and warnings are stored in the field err of the pbar struct. Errors and warnings defined are:

* ERR\_FILEOPEN: unable to open file (it does not exist or access is denied);
* ERR\_FILEALLOC: unable to alloc disk space for the file;
* ERR\_MMAP: unable to map file in memory;
* WARN\_PERC: pbar\_draw has been called with a value of argi greater than Num; perc and nblks are set to 0;
* WARN\_LEN: A bar with length less than 3 has been requested; a default value of 8 is used instead;
* ERR\_MEM: pbar\_draw has been called but bar field is zero;
* WARN\_NUM: pbar\_init has been called with Num=0; a default value of one million is used instead;

These errors and warnings are or-ed together. To check for a specific one, one must "and" err and the error/warning searched. For example to check for ERR\_FILEOPEN, do: `if((progbar->err&ERR_FILEOPEN)!=0)`.

## Examples <a name="Examples"></a>
File main.c is an example program that shows how to use the pbar library (file xmmrand.c, stored in tests/lib/, is a dependance). It uses a random number generator to calculate pi and displays the progress bar on file reso.log. One can launch the example program and watch the file showing the advancing pbar. At the end the calculated value of pi is shown.
File testcpp.cpp does the same in C++.
File test.f90 is a very simple example program in Fortran.

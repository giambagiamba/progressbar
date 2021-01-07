# Progress bar
Simple C library for drawing a progress bar on a memory mapped log file. It is designed to introduce as little overhead as possible.

## Content of the library
The pbar structured type is defined. It contains:

* char\* bar: A string with the content of the progress bar to be drawn;
* unsigned int len: length of the bar, i.e. the number of blocks to be drawn at 100%;
* uint64\_t Num: Number of iterations of the job related to the bar;
* unsigned int perc: percentage, set by caller;
* unsigned int nblks: number of full blocks, calculated internally from percentage;
* int err: error code;
* int file: log file where the bar is drawn;
* double start: initial time used for ETA calculation.

Four functions are available: pbar\_init, pbar\_eta, pbar\_draw, pbar\_close. They are all void-returning.

* void pbar\_init(pbar\* progbar, char\* filename, uint64\_t Num, unsigned int len): It opens filename and maps it in memory (using open, fallocate and mmap), and initializes the content of pbar according to its arguments. i, perc, err and nblks are set to 0.
* void pbar\_eta(pbar\* progbar, uint64\_t argi): It calculates and prints ETA. It is called by pbar\_draw.
* void pbar\_draw(pbar\* progbar, uint64\_t argi): It updates progbar-\>nblks and perc according to argi, and draw the bar, the new percentage and ETA. In case the new calculated percentage is the same stored in progbar, it returns. The same is done after checking the new calculated value of nblks.
* pbar\_close: It unmaps the file and closes it.

## How to use
* Declare a variable of type pbar and set fields len, perc and file.
* Call pbar\_init passing as first argument the pointer to the pbar variable and the others according to your choice. In case of error, pbar.err will be set (see *Errors* section).
* In your job cycle, update the bar by calling pbar\_draw. It is suggested to avoid to do the call every cycle, instead check that the calculated percentage is different from the previous value. Calling pbar\_draw with the same value of percentage won't change the bar but introduces the overhead of the call uselessly.
* When done, call pbar\_close to unmap the file and close it.

## Errors
Errors and warnings are stored in the field err of the pbar struct. Errors and warnings defined are:

* ERR\_FILEOPEN: unable to open file (it does not exist or access is denied);
* ERR\_FILEALLOC: unable to alloc disk space for the file;
* ERR\_MMAP: unable to map file in memory;
* WARN\_PERC: pbar\_draw has been called with a value of argi greater than Num; perc and nblks are set to 0;
* WARN\_LEN: A bar with length less than 3 has been requested; a default value of 8 is used instead;
* ERR\_MEM: pbar\_draw has been called but bar field is zero;
* WARN\_NUM: pbar\_init has been called with Num=0; a default value of one million is used instead;

These errors and warnings are or-ed together. To check for a specific one, one must "and" err and the error/warning searched. For example to check for ERR\_FILEOPEN, do: if((progbar-\>err&ERR\_FILEOPEN)!=0).

## Example 
File main.c is an example program that shows how to use the pbar library (file xmmrand.c is a dependance). It uses a random number generator to calculate pi and displays the progress bar on file reso.log. One can launch the example program and watch the file showing the advancing pbar. At the end the calculated value of pi is shown.

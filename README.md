# Progress bar
Simple C library for drawing a progress bar on a memory mapped log file. It is designed to introduce as little overhead as possible.

## Content of the library
The pbar structured type is defined. It contains:

* char bar: A string with the content of the progress bar to be drawn;
* unsigned int len: length of the bar, i.e. the number of blocks to be drawn at 100%;
* unsigned int perc: percentage, set by caller;
* unsigned int nblks: number of full blocks, calculated internally from percentage;
* int err: error code;
* int file: log file where the bar is drawn.

Three functions are available: pbar\_init, pbar\_draw, pbar\_close. They are all void-returning.

* pbar\_init: It opens pbar.file and maps it in memory (using open, fallocate and mmap), and initializes the content of pbar.bar according to pbar.perc and pbar.len.
* pbar\_draw: It updates pbar.nblks according to the content of pbar.perc and draw the new percentage. If the number of blocks (pbar.nblks) is different from the previous value, the bar is updated.
* pbar\_close: It unmaps the file and closes it.

## How to use
* Declare a variable of type pbar and set fields len, perc and file. Please leave bar uninitialized, it is a pointer and will be assigned by mmap.
* Call pbar\_init passing as argument the pointer to the pbar variable. In case of error, pbar.err will be set (see *Errors* section).
* In your job cycle, update the content of pbar.perc and call pbar\_draw. It is suggested to avoid to do the call every cycle, instead check that the calculated percentage is different from the previous value. Calling pbar\_draw with the same value of percentage won't change the bar but introduces the overhead of the call uselessly.
* When done, call pbar\_close to unmap the file and close it.

## Errors
Work in progress

## Example 
File main.c is an example program that shows how to use the pbar library (file xmmrand.c is a dependance). It uses a random number generator to calculate pi and displays the progress bar on file reso.log. One can launch the example program and watch the file showing the advancing pbar. At the end the calculated value of pi is shown.

/**************************************************************
* Class:  CSC-415-01 Fall 2023
* Names: Collins Gichohi, Louis Houston, Komaldeep Kaur,
* 			Raymond Liu, Aleia Natividad
* Student IDs: 922440815, 922379442, 920198887, 916624142, 
*				922439437
* GitHub Name: gsnilloC, LouisHouston, komalkaaur, Airray117
*				leileigoose
* Group Name: The Strugglers
* Project: Basic File System
*
* File: b_io.h
*
* Description: Interface of basic I/O functions
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

typedef int b_io_fd;

b_io_fd b_open (char * filename, int flags);
int b_read (b_io_fd fd, char * buffer, int count);
int b_write (b_io_fd fd, char * buffer, int count);
int b_seek (b_io_fd fd, off_t offset, int whence);
int b_close (b_io_fd fd);

#endif


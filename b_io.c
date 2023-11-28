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
* File: b_io.c
*
* Description: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"

#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
	{
	/** TODO add al the information you need in the file control block **/
	char * buff;		//holds the open file buffer
	int filePos;
	int numOfBlocks;
	int currBlock;
	int buflen;		//holds how many valid bytes are in the buffer
	char *filename;
    int fileDescriptor;
	int fileLength;
    int flags;
	} b_fcb;
 

b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].buff = NULL; //indicates a free fcbArray
		}
	
	startup = 1;
	}

//Method to get a free FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].buff == NULL)
			{
			return i;		//Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open(char *filename, int flags)
{
    b_io_fd returnFd;

    if (startup == 0)
        b_init(); // Initialize our system

    returnFd = b_getFCB(); // get our own file descriptor

    // check for error - all used FCB's
    if (returnFd == -1)
    {
        fprintf(stderr, "Error: Maximum number of open files reached\n");
        return -1;
    }

    //TODO: handle opening the file 
	
    fcbArray[returnFd].buff = (char *)malloc(B_CHUNK_SIZE);
    fcbArray[returnFd].buflen = 0;
	fcbArray[returnFd].filePos = 0;
	fcbArray[returnFd].numOfBlocks;
	fcbArray[returnFd].currBlock = 0;
	fcbArray[returnFd].buflen = B_CHUNK_SIZE;		//holds how many valid bytes are in the buffer
	fcbArray[returnFd].filename = filename;
    fcbArray[returnFd].fileDescriptor = returnFd;
	fcbArray[returnFd].fileLength;
    fcbArray[returnFd].flags = flags;

    return returnFd; // all set
}



// Interface to seek function	
int b_seek (b_io_fd fd, off_t offset, int whence)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
	
	off_t newOffset;
    switch (whence) {
        case 0:
            newOffset = offset;
            break;
        case 1:
            newOffset = fcbArray[fd].filePos + offset;
            break;
        case 2:
            newOffset = fcbArray[fd].fileLength + offset;
            break;
        default:
            return -1;  // Invalid whence parameter
    }

	return (newOffset); //Change this
	}



// Interface to write function	
int b_write (b_io_fd fd, char * buffer, int count)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
		
	return (0); //Change this
	}



// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill 
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read (b_io_fd fd, char * buffer, int count)
	{

	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	return (0);	//Change this
	}
	
// Interface to Close the file	
int b_close (b_io_fd fd)
	{
		if (fcbArray[fd].buff != NULL) {
        free(fcbArray[fd].buff);
        fcbArray[fd].buff = NULL;
    } 

		fcbArray[fd].filename = NULL;
	}

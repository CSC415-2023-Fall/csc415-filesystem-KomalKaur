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
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"
#include "VCB.c"

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
		struct vcb* tempVCB = (struct vcb*)malloc(blockSize);

		LBAread(tempVCB,1, 0);

		if(tempVCB->Signature == SIGNATURE){ // checking if signature matches
			return 0; // volume already is initalized
		}else {
			initVCB(numberOfBlocks, blockSize);
		}

	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */

	return 0;
	}

void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Names: 
* Student IDs:
* GitHub Name:
* Group Name:
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
		printf("Running initFileSystem...\n");
		struct vcb* tempVCB = (struct vcb*)malloc(blockSize);
		printVCBinf(tempVCB);
		LBAread(tempVCB,1, 0);

		// if(tempVCB->Signature == SIGNATURE){ // checking if signature matches
		// 	printf("tempSig: %lld vs ourSig: %d\n", tempVCB->Signature, SIGNATURE);
		// 	printVCBinf(tempVCB);
		// 	return 0; // volume already is initalized
		// }else {
		// 	printf("Gotta initVCB...\n");
		// 	initVCB(numberOfBlocks, blockSize);
		// }
		initVCB(numberOfBlocks, blockSize);

	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */

	return 0;
	}

void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
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
#include "directories.h"
#include "mfs.c"

int initFileSystem(uint64_t numberOfBlocks, uint64_t blockSize)
{
	printf("Initializing File System with %ld blocks with a block size of %ld\n\n", numberOfBlocks, blockSize);

	struct vcb *tempVCB = (struct vcb *)malloc(blockSize);

	LBAread(tempVCB, 1, 0);

	int sizeOfMapBytes = (numberOfBlocks / 8) + 1;
    int sizeOfMapBlocks = (sizeOfMapBytes + blockSize - 1) / blockSize;


	if (tempVCB->Signature == SIGNATURE)
	{
		loadFreeSpace(numberOfBlocks, blockSize);
		loadRootDir();
		loadCWD();
		fs_mkdir("/Users", 1);

		//initVCB(numberOfBlocks, blockSize);
		return 0; // volume already is initalized
	}
	else
	{
		initVCB(numberOfBlocks, blockSize);
	}

	free(tempVCB);

	return 0;
}

void exitFileSystem()
{
	free(rootDir);
	free(freeSpaceMap);
	printf("System exiting\n");
}
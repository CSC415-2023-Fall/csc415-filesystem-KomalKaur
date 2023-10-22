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


int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	/*
	struct vcb{
    char nameOfVolume[VOLUME_NAME_SIZE];
    unsigned int totalBlocks;
    unsigned int freeBlockCount;
    size_t blockSize;
    char signature[SIGNATURE_SIZE];
    int freeSpaceMap[MAX_BLOCKS][MAX_SECTORS];
} vcb;
*/
	struct vcb vcbMain;
	strcpy(vcbMain.nameOfVolume, ":S");
	vcbMain.totalBlocks = numberOfBlocks;
	vcbMain.blockSize = blockSize;
	strcpy(vcbMain.signature, SIGNATURE);
	vcbMain.freeBlockCount = numberOfBlocks;
	for(int i = 0; i < MAX_BLOCKS;i++){
		for(int j = 0; j < MAX_SECTORS; j++){
			vcbMain.freeSpaceMap[i][j] = 1; // indicative of a free block
		}
	}
	// TODO:somewhere where have to malloc block b 
	return 0;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}
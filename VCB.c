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
* File: VCB.c
*
* Description: Basic File System - Volume Control Block
* Functions used to initialize Volume control block which also
* initializes the free space and the root directory. 
**************************************************************/
#include "VCB.h"

int initVCB(uint64_t numberOfBlocks, uint64_t blockSize){
	printf("---------------- FORMATTING ---------------- \n");
    struct vcb* vcbMain = (struct vcb*)malloc(blockSize); // malloc a pointer to vcb
	if(vcbMain == NULL){
		perror("Failed to allocate mem for vcb pointer");
		return 1; // return an error;
	}
	strcpy(vcbMain->volumeName, ":S"); // Volume Name
	vcbMain->totalBlocks = numberOfBlocks; // totalBlocks
	vcbMain->blockSize = blockSize; // blocksize delcaration
	vcbMain->Signature = SIGNATURE; // signature from hex: 0x4C4F4C41
	vcbMain->freeBlockCount = numberOfBlocks; // because its empty rn
	vcbMain->locFreeSpaceBitMap = initFreeSpace(numberOfBlocks, blockSize);
    vcbMain-> locRootDir = initDirectory(10, blockSize, NULL);

    if(LBAwrite(vcbMain, 1, 0)){
		free(vcbMain);
        return 0;
    }
	
}
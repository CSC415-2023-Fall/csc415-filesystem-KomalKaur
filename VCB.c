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
*
**************************************************************/
#include "VCB.h"

void printVCBinf(struct vcb* t1);

int initVCB(uint64_t numberOfBlocks, uint64_t blockSize){
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
		printVCBinf(vcbMain);
		free(vcbMain);
        return 0;
    }
	
}

void printVCBinf(struct vcb* t1){
	printf("~~VCB~~~\n");
	printf("VCB volume name: %s\n", t1->volumeName);
	printf("Total Block: %ld\n", t1->totalBlocks);
	printf("freeBlockCount: %ld\n", t1->freeBlockCount);
	printf("locFreeSpaceBitMap %i\n", t1->locFreeSpaceBitMap);
	printf("loc RootDir: %i\n", t1->locRootDir);
	printf("block size from VCB: %ld\n", t1->blockSize);
	printf("Signature VCB: %lld\n", t1->Signature);
}
#include "VCB.h"
#include "freespace.c"
#include "directories.c"

void printVCBinf(struct vcb* t1);

int initVCB(uint64_t numberOfBlocks, uint64_t blockSize){
	printf("Running initVCB...\n");
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
	printVCBinf(vcbMain);
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
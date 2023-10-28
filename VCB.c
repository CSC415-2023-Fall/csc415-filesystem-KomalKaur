#include <stddef.h>
#include <string.h>

#include "VCB.h"


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
	return 0;
}
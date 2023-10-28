#include <stddef.h>
#include <string.h>
#include "fsLow.h"

// Defining 
#define VOLUME_NAME_SIZE 100
#define SIGNATURE 0x4C4F4C41
#define SIGNATURE_SIZE 11

struct vcb{
    char volumeName[VOLUME_NAME_SIZE]; // name of the volume
    u_int64_t totalBlocks; 
    u_int64_t freeBlockCount;
    u_int32_t locFreeSpaceBitMap;
    u_int32_t locRootDir;
    size_t blockSize;
    ull_t Signature;
} vcb;

int initVCB(uint64_t , uint64_t);
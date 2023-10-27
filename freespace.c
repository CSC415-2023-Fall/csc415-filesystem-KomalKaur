/**************************************************************
 * Class:  CSC-415-01 Fall 2023
 * Names:
 * Student IDs:
 * GitHub Name:
 * Group Name:
 * Project: Basic File System
 *
 * File: freespace.c
 *
 * Description: Basic File System: Freespace Map
 *
 **************************************************************/

#include "freespace.h"
#include "b_io.h"

#define MAX_NUMBER_OF_BLOCKS 19531
#define BLOCK_SIZE 512

// initialize the free space map and return its starting block int initFreespaceMap()
int initFreeSpace()
{

    int sizeOfMapBytes = (MAX_NUMBER_OF_BLOCKS / 8) + 1;
    int sizeOfMapBlocks = ((sizeOfMapBytes) / BLOCK_SIZE) + 1;
    
    uint8_t *freeSpaceMap = (uint8_t *)malloc(sizeOfMapBytes);
    
    // if (freeSpaceMap == NULL){
    //     return -1;
    // }

    for (int i = 0; i < MAX_NUMBER_OF_BLOCKS; i++)
    {
        freeSpaceMap[i] = 0;
    }

    for (int i = 0; i < sizeOfMapBlocks; i++)
    {
        setBit(*freeSpaceMap, i);
    }

    int blocksWritten = LBAwrite(freeSpaceMap, sizeOfMapBlocks, 1);

    if (blocksWritten != sizeOfMapBlocks){
        printf("Error writing blocks");
    }

    return 1;
}

setBit(uint8_t *freeSpaceMap, int n)
{
    int byteIndex = n / 8;
    int bitIndex = n % 8;
    uint8_t bitmask = 1 << (7 - bitIndex);

    freeSpaceMap[byteIndex] |= bitmask;
}

clearBit(uint8_t *freeSpaceMap, int n)
{
    int byteIndex = n / 8;
    int bitIndex = n % 8;
    uint8_t bitmask = 1 << (7 - bitIndex);
    bitmask = ~bitmask;

    freeSpaceMap[byteIndex] &= bitmask;
}
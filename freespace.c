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
 * File: freespace.c
 *
 * Description: This file contains our free space manager and
 * all the helper functions needed.
 *
 **************************************************************/

#include "freespace.h"

uint8_t *freeSpaceMap;
uint64_t maxNumberOfBlocks;
uint64_t bytesPerBlock;

// initialize free space map with first blocks marked as used
// if bit is set to 0 it is free, if bit is set to 1 it is taken
int initFreeSpace(uint64_t numberOfBlocks, uint64_t blockSize)
{
    maxNumberOfBlocks = numberOfBlocks;
    bytesPerBlock = blockSize;

    // calculate how big our free space map will be in bytes so we can allocate precise amount
    int sizeOfMapBytes = (maxNumberOfBlocks / 8) + 1;
    // calculate how many blocks will be used by our free space map
    int sizeOfMapBlocks = (sizeOfMapBytes + blockSize - 1) / blockSize;

    // allocate bytes for map
    freeSpaceMap = (uint8_t *)malloc(sizeOfMapBytes);

    // check if malloc succeeded
    if (freeSpaceMap == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    // Set all bits to 0 (indicating all blocks as free)
    for (int i = 0; i < sizeOfMapBytes; i++)
    {
        freeSpaceMap[i] = 0;
    }

    // set the first bits as used because they hold the VCB and the free space map itself
    for (int i = 0; i <= sizeOfMapBlocks; i++)
    {
        setBit(i);
    }

    // write blocks to disk and check return value of LBAwrite
    int blocksWritten = LBAwrite(freeSpaceMap, sizeOfMapBlocks, 1);
    if (blocksWritten != sizeOfMapBlocks)
    {
        fprintf(stderr, "Error writing blocks to disk\n");
        free(freeSpaceMap); // Free allocated memory before returning
        return -1;
    }

    // free(freeSpaceMap);
    return 1;
}

// Allocate blocks method similar to Professor Bierman's example. The function will take number
// of blocks requested and how many blocks should be in each extent. If the numbers are the same
// this will ensure contigous allocation if possible. Will return an array of extents that
// give start and count of contigous blocks.
extent *allocateBlocks(int numBlocks, int minBlocksInExtent)
{
    // allocate memory for the array of extents which is numBlocks / minBlocksInExtent + 2 just in
    // case
    extent *extentTable = (extent *)malloc(sizeof(extent) *
                                           (numBlocks / minBlocksInExtent + 2));

    // check if malloc succeeded
    if (extentTable == NULL)
    {
        fprintf(stderr, "Memory allocation for extents failed\n");
        return NULL;
    }

    // number of blocks that have been set so far
    int blockCount = 0;
    // index of extent within extent table
    int extentIndex = 0;
    // where extent starts from
    int startBlock = 0;

    // loop until all requested blocks have been allocated
    while (blockCount < numBlocks)
    {
        int consecutiveFreeBlocks = 0;
        // check for consecutive free blocks to allocate extents with
        for (int i = startBlock; i < maxNumberOfBlocks; ++i)
        {
            if (!checkBit(i))
            {
                if (consecutiveFreeBlocks == 0)
                {
                    startBlock = i;
                }
                // if free increment consecutive free blocks count
                consecutiveFreeBlocks++;
            }
            else
            {
                // if used reset consecutive free blocks count
                consecutiveFreeBlocks = 0;
            }

            // if the number of consecutive free blocks satisfies minBlocks extent, add to the
            // extent table and set the bits used
            if (consecutiveFreeBlocks >= minBlocksInExtent)
            {
                // set the extents start and count
                extentTable[extentIndex].start = startBlock;
                extentTable[extentIndex].count = consecutiveFreeBlocks;
                // increment to fresh extent for next blocks
                extentIndex++;
                // keep track of how many blocks have been allocated thuse far
                blockCount += consecutiveFreeBlocks;

                // this how many bits need to be set to signify the newly allocated blocks
                int totalBitsSet = startBlock + consecutiveFreeBlocks;

                // Mark allocated blocks as used
                for (int j = startBlock; j < totalBitsSet; ++j)
                {
                    setBit(j);
                }

                break;
            }
        }

        if (consecutiveFreeBlocks == 0)
        {
            // No more free blocks found
            break;
        }
    }

    // Write the updated map to disk after block allocation
    int blocksWritten = LBAwrite(freeSpaceMap, 5, 1);

    // debug
    // printf("# of extents: %d\n", extentIndex);
    // Set the last extent as indicator for the end of the array of extents
    extentTable[extentIndex].start = 0;
    extentTable[extentIndex].count = 0;

    // return extent table after allocation
    return extentTable;
}

int loadMap(uint64_t numberOfBlocks, uint64_t blockSize)
{
    int sizeOfMapBytes = (numberOfBlocks / 8) + 1;
    int sizeOfMapBlocks = (sizeOfMapBytes + blockSize - 1) / blockSize;

    // allocate bytes for map
    freeSpaceMap = (uint8_t *)malloc(sizeOfMapBytes);


    //LBAread(freeSpaceMap, sizeOfMapBlocks, 1);
    return 0;
}

int free_blocks(extent *extentTable)
{
    if (extentTable == NULL)
    {
        return -1;
    }

    int i = 0;

    while (extentTable[i].start != 0 || extentTable->count != 0)
    {
        int startBlock = extentTable[i].start;
        int endBlock = startBlock + extentTable[i].start;

        for (int j = startBlock; j < endBlock; j++)
        {
            clearBit(j);
        }

        i++;
    }

    int blocksWritten = LBAwrite(freeSpaceMap, 5, 1);

    if (blocksWritten != 1)
    {
        fprintf(stderr, "Error writing free space map to disk\n");
        return -1;
    }

    return 0;
}

// helper function to set bits to used
void setBit(int n)
{
    int byteIndex = n / 8;
    int bitIndex = n % 8;
    uint8_t bitmask = 1 << (7 - bitIndex);
    freeSpaceMap[byteIndex] |= bitmask;
}

// helper function to set bits to free
void clearBit(int n)
{
    int byteIndex = n / 8;
    int bitIndex = n % 8;
    uint8_t bitmask = ~(1 << (7 - bitIndex));
    freeSpaceMap[byteIndex] &= bitmask;
}

// helper function to check if bit is free or used
int checkBit(int n)
{
    int byteIndex = n / 8;
    int bitIndex = n % 8;
    return (freeSpaceMap[byteIndex] & (1 << (7 - bitIndex))) != 0;
}

// ./Hexdump/hexdump.linuxM1 SampleVolume --start 64 --count 10
// debug function to print
void printBitMap()
{
    for (int i = 0; i < 20; i++)
    {
        if (i % 64 == 0 && i != 0)
        {
            printf("\n");
        }
        if (freeSpaceMap[i / 8] & (1 << (7 - (i % 8))))
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
    printf("\n");
}
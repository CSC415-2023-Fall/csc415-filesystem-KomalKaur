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
    int blocksWritten = LBAwrite(freeSpaceMap, (maxNumberOfBlocks / 8) + 1, 1);
    if (blocksWritten != (maxNumberOfBlocks / bytesPerBlock + 1))
    {
        fprintf(stderr, "Error writing blocks to disk\n");
        // Handle any necessary cleanup or error handling
        return NULL; // Or any appropriate error indicator
    }

    // debug
    // printf("# of extents: %d\n", extentIndex);
    // Set the last extent as indicator for the end of the array of extents
    extentTable[extentIndex].start = 0;
    extentTable[extentIndex].count = 0;

    // return extent table after allocation
    return extentTable;
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

// debug function to print
void printBitMap()
{
    for (int i = 0; i < maxNumberOfBlocks; i++)
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
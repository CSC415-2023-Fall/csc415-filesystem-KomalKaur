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
#include <stddef.h>

// Define the size of the free space map in bytes
#define MAP_SIZE 2442
#define BLOCK_SIZE 512

// initialize the free space map and return its starting blck
unsigned int initializeFreeSpace()
{
    // calculate number of blocks in free space map and make sure it is rounded up properly
    unsigned int numberOfBlocks = (MAP_SIZE + BLOCK_SIZE - 1 / BLOCK_SIZE);

    // allocate memory for the map its self
    uint8_t *map = (uint8_t *)malloc(numberOfBlocks * BLOCK_SIZE);

    // check if allocation was successful
    if (map == NULL)
    {
        return -1;
    }

    // Initialize the free space map with the first 6 blocks marked as used by the VCB and the map itself
    for (int i = 0; i < 6 * BLOCK_SIZE * 8; i++)
    {
        // Calculate the byte index in the freeSpaceMap array
        int byteIndex = i / 8;

        // Calculate the bit index within the byte (0 to 7)
        int bitIndex = i % 8;

        // Create a bitmask with a single 1 at the bitIndex position (e.g., 00001000)
        uint8_t bitmask = 1 << bitIndex;

        // Use the bitwise OR-assignment operator to set the bit to 1 in the byte
        map[byteIndex] |= bitmask;
    }

    // starting block of the map is 1 because the VCB takes up the 0th block
    unsigned int startingBlock = 1;

    // write map to disk
    LBAwrite(map, numberOfBlocks, startingBlock);

    // free map since it has been written to disk
    free(map);

    // return location of the beginning of the map
    return startingBlock;
}

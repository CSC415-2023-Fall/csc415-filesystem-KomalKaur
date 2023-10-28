/**************************************************************
 * Class:  CSC-415-01 Fall 2023
 * Names:
 * Student IDs:
 * GitHub Name:
 * Group Name:
 * Project: Basic File System
 *
 * File: freespace.h
 *
 * Description: Basic File System: Freespace Map
 *
 **************************************************************/


#ifndef FREESPACE_H
#define FREESPACE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

typedef struct extent {
    int start;
    int count;
} extent, * pextent;

// called to when volume is initialized. returns block number
// of where map starts
int initFreeSpace ();

// if volume is already initialized load the free space map
// from disk so it is in memory and ready to use
int loadFreeSpace (int blockCount, int bytesPerBlock);

// allocateblocks is how you obtain disk blocks. the first parameter is the number of
// blocks the caller requires. The minPerExtent (second) parameter is the minimum number
// of blocks in any one extent (or at least all but the last extent);
// If the two parameters are the same value, then the
// resulting allocation will be contiguous in one extent.
// The return value is an array of extents the last one has the start and count = 0
// which indicates the end of the array of extents.
// The caller is responsible for calling free on the return value.
extent * allocateBlocks(uint8_t *map, unsigned int numBlocksToAllocate);

void freeBlocks(int start, int count);


#endif

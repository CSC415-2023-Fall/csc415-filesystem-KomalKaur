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
#include "fsLow.h"

typedef struct extent {
    int start;
    int count;
} extent, * pextent;

uint8_t *freeSpaceMap = NULL; 
uint64_t maxNumberOfBlocks;
uint64_t bytesPerBlock;


// called to when volume is initialized. returns block number
// of where map starts
int initFreeSpace(uint64_t numberOfBlocks, uint64_t blockSize);

// if volume is already initialized load the free space map
// from disk so it is in memory and ready to use
int loadFreeSpace (int blockCount, int bytesPerBlock);

// allocateblocks is how you obtain disk blocks
extent * allocateBlocks(int numBlocks, int minBlocksInExtent);

void freeBlocks(int start, int count);
void setBit(int n);  // set a specific bit to used
void clearBit(int n);  // set a specific bit to free
int checkBit(int n);  // check if bit is used or free
void printBitMap();


#endif

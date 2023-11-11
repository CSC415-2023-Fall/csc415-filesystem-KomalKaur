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
* File: freespace.h
*
* Description: This header files contains the function 
* definitions for the functions we will be using.
*
**************************************************************/


#ifndef FREESPACE_H
#define FREESPACE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include "fsLow.h"

// struct used to store extents with a start block and count of how many blocks extent is
typedef struct extent {
    int start;
    int count;
} extent, * pextent;

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

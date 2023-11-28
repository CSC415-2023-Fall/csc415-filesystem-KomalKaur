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
* File: VCB.h
*
* Description: Basic File System - Volume Control Block
* Interface of functions needed to initialize our volume 
* control block.
*
**************************************************************/
#include <stddef.h>
#include <string.h>
#include "fsLow.h"
#include "freespace.c"
#include "directories.c"

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
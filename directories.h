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
* File: directories.h
*
* Description: 
* Header file holds the definitions used to initialize a 
* directory.
**************************************************************/
#ifndef DIRECTORIES_H
#define DIRECTORIES_H

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "freespace.h"
#include "b_io.h"
#include "fsLow.h"
#include "directories.h"
#include "mfs.h"

#define MAX_FILE_NAME 256

typedef struct {
    char fileName[MAX_FILE_NAME];
    unsigned long size;
    extent * extentTable;
    time_t lastModified;
    time_t lastAccessed;
    time_t timeCreated;
    char isDirectory;
    mode_t permissions;
} DirEntry;


typedef struct ppInfo {
	DirEntry * parent;
	int index;
	char * lastElement;
	//isPathValid();
} ppInfo;

int initDirectory(int initialDirEntries, uint64_t blockSize, DirEntry *parent);
int loadRootDir();
int parsePath (char * pathname, ppInfo * ppi);
int findEntryInDir(DirEntry *directory, char *entryName);
DirEntry *LoadDir(DirEntry *entry);
int isDir(DirEntry *entry);
int findEmptySpotInDir(DirEntry *);
int isDirEmtpy(DirEntry *);
int setDot(DirEntry * entry);

#endif

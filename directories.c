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
* File: directories.c
*
* Description: 
* This file contains the initDirectory() funtction which 
* intializes a directory. It sets inital metadata for each 
* entry and handles the "." and ".." structure for file
* systems. The directory is then written to disk
**************************************************************/

#include "directories.h"

int initDirectory(int initialDirEntries, uint64_t blockSize, DirEntry *parent)
{
    // there has to be at least 1 directory entry
    if (initialDirEntries <= 0)
    {
        printf("Error: Invalid number of initial directory entries.\n");
        return -1;
    }

    // calculate size of directory in both blocks and bytes
    int rootDirSizeBytes = initialDirEntries * sizeof(DirEntry);
    int rootDirSizeBlocks = (rootDirSizeBytes + (blockSize - 1) / blockSize);

    rootDirSizeBytes = rootDirSizeBlocks * blockSize;

    int actualDirEntries = rootDirSizeBytes / sizeof(DirEntry);

    // allocate memory for the directory entries
    DirEntry *directoryEntries = malloc(actualDirEntries * sizeof(DirEntry));

    // check if malloc succeeded
    if (directoryEntries == NULL)
    {
        printf("Error: Memory allocation failed for directory entries.\n");
        return -1;
    }

    // Initialize directory entries
    for (int i = 0; i < actualDirEntries; ++i)
    {
        directoryEntries[i].fileName[0] = '\0';
        directoryEntries[i].size = 0;
        directoryEntries[i].extentTable = NULL;
        directoryEntries[i].lastModified = 0;
        directoryEntries[i].lastAccessed = 0;
        directoryEntries[i].timeCreated = 0;
        directoryEntries[i].isDirectory = 0;
    }

    time_t t = time(NULL);

    // create root directory
    strcpy(directoryEntries[0].fileName, ".");
    directoryEntries[0].size = actualDirEntries * sizeof(DirEntry);
    directoryEntries[0].isDirectory = 1;
    directoryEntries[0].timeCreated = t;
    directoryEntries[0].lastAccessed = t;
    directoryEntries[0].lastModified = t;
    directoryEntries[0].extentTable =  allocateBlocks(rootDirSizeBlocks, rootDirSizeBlocks);

    // return start block of where directory entries start on disk
    int startBlock = directoryEntries[0].extentTable->start;

    DirEntry *firstEntryPtr;
    
    // handle root directory case 
    if (parent != NULL)
    {
        firstEntryPtr = parent;
    }
    else if (parent == NULL)
    {
        firstEntryPtr = &directoryEntries[0];

        // Copy details for the ".." directory entry (parent directory)
        strcpy(directoryEntries[1].fileName, "..");
        directoryEntries[1].size = firstEntryPtr->size;
        directoryEntries[1].isDirectory = firstEntryPtr->isDirectory;
        directoryEntries[1].timeCreated = firstEntryPtr->timeCreated;
        directoryEntries[1].lastAccessed = firstEntryPtr->lastAccessed;
        directoryEntries[1].lastModified = firstEntryPtr->lastModified;
        directoryEntries[1].extentTable = firstEntryPtr->extentTable;
    }

    // write it to disk
    LBAwrite(directoryEntries, rootDirSizeBlocks, startBlock);
    free(directoryEntries);

    return startBlock; // Return start block of directory entries
}

int parsePath (char * pathname, ppInfo * ppi){
    if (pathname == NULL || ppi == NULL){
        return -1;
    }


    return 0;
}
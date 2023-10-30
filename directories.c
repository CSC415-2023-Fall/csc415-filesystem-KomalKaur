#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "freespace.h"
#include "b_io.h"
#include "fsLow.h"
#include "directories.h"

int initDirectory(int initialDirEntries, uint64_t blockSize, DirEntry *parent)
{
    if (initialDirEntries <= 0)
    {
        printf("Error: Invalid number of initial directory entries.\n");
        return -1;
    }

    int rootDirSizeBytes = initialDirEntries * sizeof(DirEntry);
    int rootDirSizeBlocks = (rootDirSizeBytes + (blockSize - 1) / blockSize);

    rootDirSizeBytes = rootDirSizeBlocks * blockSize;

    int actualDirEntries = rootDirSizeBytes / sizeof(DirEntry);

    DirEntry *directoryEntries = malloc(actualDirEntries * sizeof(DirEntry));

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

    strcpy(directoryEntries[0].fileName, ".");
    directoryEntries[0].size = actualDirEntries * sizeof(DirEntry);
    directoryEntries[0].isDirectory = 1;
    directoryEntries[0].timeCreated = t;
    directoryEntries[0].lastAccessed = t;
    directoryEntries[0].lastModified = t;
    directoryEntries[0].extentTable =  allocateBlocks(rootDirSizeBlocks, rootDirSizeBlocks);

    int startBlock = directoryEntries[0].extentTable->start;

    DirEntry *firstEntryPtr;
    
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

    LBAwrite(directoryEntries, rootDirSizeBlocks, startBlock);
    free(directoryEntries);

    return startBlock; // Return 0 indicating successful initialization
}

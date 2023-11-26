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
#include <stdlib.h>

DirEntry *rootDir;
DirEntry *cwd;
DirEntry *dot;

int initDirectory(int initialDirEntries, uint64_t blockSize, DirEntry *parent)
{
    // there has to be at least 1 directory entry
    if (initialDirEntries <= 0)
    {
        printf("Error: Invalid number of initial directory entries.\n");
        return -1;
    }

    int sizeOfDE = sizeof(DirEntry);

    int rootDirSizeBytes = initialDirEntries * sizeOfDE;
    int rootDirSizeBlocks = (initialDirEntries * sizeOfDE + blockSize - 1) / blockSize;

    int actualDirEntries = rootDirSizeBytes / sizeof(DirEntry);

    // printf("\nrootDirSizeBytes: %d\n", rootDirSizeBytes);
    // printf("rootDirSizeBlocks: %d\n", rootDirSizeBlocks);
    // printf("Size of DE: %d \n",  sizeof(DirEntry));
    // printf("ACTUAL: %d\n", actualDirEntries);

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
        directoryEntries[i].directoryStartBlock = -1;
    }

    time_t t = time(NULL);

    // create new directory
    strcpy(directoryEntries[0].fileName, ".");
    directoryEntries[0].size = actualDirEntries * sizeof(DirEntry);
    directoryEntries[0].isDirectory = 1;
    directoryEntries[0].timeCreated = t;
    directoryEntries[0].lastAccessed = t;
    directoryEntries[0].lastModified = t;
    directoryEntries[0].extentTable = allocateBlocks(rootDirSizeBlocks, rootDirSizeBlocks);
    directoryEntries[0].directoryStartBlock = directoryEntries[0].extentTable->start;

    // return start block of where directory entries start on disk
    int startBlock = directoryEntries[0].extentTable->start;

    printBitMap();

    DirEntry *firstEntryPtr;

    // handle root directory case
    if (parent != NULL)
    {
        firstEntryPtr = parent;

        strcpy(directoryEntries[1].fileName, "..");
        directoryEntries[1].size = firstEntryPtr->size;
        directoryEntries[1].isDirectory = firstEntryPtr->isDirectory;
        directoryEntries[1].timeCreated = firstEntryPtr->timeCreated;
        directoryEntries[1].lastAccessed = firstEntryPtr->lastAccessed;
        directoryEntries[1].lastModified = firstEntryPtr->lastModified;
        directoryEntries[1].extentTable = firstEntryPtr->extentTable;
        directoryEntries[1].directoryStartBlock = firstEntryPtr->directoryStartBlock;
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
        directoryEntries[1].directoryStartBlock = firstEntryPtr->directoryStartBlock;
    }

    // write it to disk
    LBAwrite(directoryEntries, rootDirSizeBlocks, startBlock);

    // TODO: check if this works
    if (parent == NULL)
    {
        printf("FREED DIR ENTRIES");
        free(directoryEntries);
    }

    return startBlock; // Return start block of directory entries
}

int loadRootDir()
{
    int rootDirSize = 6 * 512;
    rootDir = (DirEntry *)malloc(rootDirSize);

    if (rootDir == NULL)
    {
        return -1;
    }

    LBAread(rootDir, 6, 6);
    return 0;
}

int loadCWD()
{
    cwd = rootDir;

    return 0;
}

int parsePath(char *pathname, ppInfo *ppi)
{
    if (pathname == NULL || ppi == NULL)
    {
        return -1;
    }

    DirEntry *startPath = malloc(sizeof(DirEntry));
    DirEntry *parent = malloc(sizeof(DirEntry));

    if (pathname[0] == '/')
    {
        startPath = rootDir;
    }
    else
    {
        startPath = cwd;
    }

    parent = startPath;

    char *token1;
    char *saveptr = NULL;

    token1 = strtok_r(pathname, "/", &saveptr);

    if (token1 == NULL)
    {
        if (strcmp(pathname, "/") == 0)
        {
            ppi->parent = parent;
            ppi->index = -1;
            ppi->lastElement = NULL;
            return 0;
        }

        return -1;
    }

    char *token2;
    while (token1 != NULL)
    {
        // look for name in directory entries one by one
        int index = findEntryInDir(parent, token1);
        token2 = strtok_r(NULL, "/", &saveptr);
        if (token2 == NULL)
        {
            ppi->parent = parent;
            ppi->lastElement = strdup(token1);
            ppi->index = index;

            return 0;
        }

        if (index == -1)
        {
            return -2;
        }

        if (!isDir(&parent[index]))
        {
            return -2;
        }

        DirEntry *temp = LoadDir(&(parent[index]));

        if (parent != startPath)
        {
            free(parent);
        }

        parent = temp;
        token1 = token2;
    }

    free(startPath);
    return 0;
}

void testParsePath(char *pathname)
{
    ppInfo *pathInfo = (ppInfo *)malloc(sizeof(ppInfo));

    int retVal = parsePath(pathname, pathInfo);

    if (retVal != 0)
    {
        printf("Parse path failed");
    }

    DirEntry *parent = malloc(sizeof(DirEntry));

    parent = pathInfo->parent;

    printf("Parent: %s\n", parent->fileName);

    printf("INDEX: %d\n", pathInfo->index);
    printf("Last Element: %s\n", pathInfo->lastElement);
}

int isDir(DirEntry *entry)
{
    if (entry == NULL)
    {
        return -1; // Error code for invalid input
    }

    return entry->isDirectory; // Return directory status (1 for directory, 0 for not a directory)
}

int findEntryInDir(DirEntry *directory, char *entryName)
{
    if (directory == NULL || entryName == NULL)
    {
        printf("INVALID INPUT");
        return -1; // Error code for invalid inputs
    }

    int numEntries = directory->size / sizeof(DirEntry);

    //printf(NUMBER OF ENTRIES": %d\n", numEntries);

    for (int i = 0; i < numEntries; i++)
    {
        if (strcmp(directory[i].fileName, entryName) == 0)
        {
            return i; // Return the index of the found directory entry
        }
    }

    return -1; // Return -1 if the entry is not found in the directory
}

int findNextAvailableEntryInDir(DirEntry *directory)
{

    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    int returnVal = parsePath("/", pathInfo);

    if (returnVal != 0)
    {
        free(pathInfo);
        printf("HERE");
        return -1;
    }

    DirEntry *entry = pathInfo->parent;

    if (entry == NULL)
    {
        printf("NO HERE");
        return -1; // Error code for invalid input
    }

    int numEntries = entry->size / sizeof(DirEntry);

    for (int i = 0; i < numEntries; i++)
    {
        if (entry[i].fileName[0] == '\0')
        {
            return i; // Return the index of the first available entry
        }
    }

    return -1; // Return -1 if no available entry is found in the directory
}

DirEntry *LoadDir(DirEntry *entry)
{
    if (entry == NULL || entry->isDirectory == 0)
    {
        return NULL; // Return NULL if the entry is not a valid directory
    }

    extent *dirExtent = entry->extentTable;

    // Calculate the number of blocks to read for the directory based on its extent information
    uint64_t blockSize = 512;
    uint64_t totalBlocksToRead = dirExtent->count;
    uint64_t startBlock = dirExtent->start;

    // Calculate the size in bytes to read
    uint64_t sizeToRead = totalBlocksToRead * blockSize;

    // Allocate memory to store the directory structure
    DirEntry *directoryStructure = (DirEntry *)malloc(sizeToRead);

    // Read the directory structure from disk using LBAread
    uint64_t blocksRead = LBAread(directoryStructure, totalBlocksToRead, startBlock);

    // Perform error handling and return NULL in case of failure
    if (blocksRead != totalBlocksToRead)
    {
        free(directoryStructure); // Free allocated memory in case of failure
        return NULL;
    }

    // Returning the directory structure
    return directoryStructure;
}

// /// @brief Finds an empty spot in the passed dirEntry
// /// @param directory
// /// @return returns the first index in the dir that is empty
// int findEmptySpotInDir(DirEntry *directory){

//     if(directory == NULL || entryName == NULL){
//         return -1;
//     }

//     int numEntries = directory->size / sizeof(DirEntry); // find the size of dir

//     // loop through to find an empty spot
//     for(int i = 0; i < numEntries; i++){
//         if(strcmp(directory[i].fileName, "") == 0){
//             return i;
//         }
//     }
//     //if none found return -1
//     return -1;
// }

/// @brief Function just to check if the dir is empty for rmdir. Acts as a boolean
/// @param directory
/// @return Returns 0 if empty, 1 if not
int isDirEmtpy(DirEntry *directory)
{

    if (directory == NULL)
    {
        return -1;
    }

    int numEntries = directory->size / sizeof(DirEntry);

    for (int i = 0; i < numEntries; i++)
    {
        if (strcmp(directory[i].fileName, "") != 0)
        {
            return 0; // not empty
        }
    }

    return 1; // empty
}

/// @brief Deletes the dirEntry by setting all of its values to initial values
/// @param directory
/// @return Returns 0 if successful returns 1 if unsuccessful
int deleteDirEntry(DirEntry *directory)
{
    if (directory == NULL)
    {
        return -1;
    }

    directory->fileName[0] = '\0';
    directory->size = 0;
    directory->extentTable = NULL;
    directory->lastModified = 0;
    directory->lastAccessed = 0;
    directory->timeCreated = 0;
    directory->isDirectory = 0;

    // series of checks to make sure the dir is now gone?
    return 0;
}
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

DirEntry *rootDir;
DirEntry *cwd;

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
    directoryEntries[0].extentTable = allocateBlocks(rootDirSizeBlocks, rootDirSizeBlocks);

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
        rootDir = firstEntryPtr;

        // Copy details for the ".." directory entry (parent directory)
        strcpy(directoryEntries[1].fileName, "..");
        directoryEntries[1].size = firstEntryPtr->size;
        directoryEntries[1].isDirectory = firstEntryPtr->isDirectory;
        directoryEntries[1].timeCreated = firstEntryPtr->timeCreated;
        directoryEntries[1].lastAccessed = firstEntryPtr->lastAccessed;
        directoryEntries[1].lastModified = firstEntryPtr->lastModified;
        directoryEntries[1].extentTable = firstEntryPtr->extentTable;

        cwd = rootDir;
    }

    // write it to disk
    LBAwrite(directoryEntries, rootDirSizeBlocks, startBlock);
    free(directoryEntries);

    return startBlock; // Return start block of directory entries
}

int parsePath(char *pathname, ppInfo *ppi)
{
    if (pathname == NULL || ppi == NULL)
    {
        return -1;
    }

    DirEntry * startPath = malloc(sizeof(DirEntry));
    DirEntry * parent = malloc(sizeof(DirEntry));

    if (pathname[0] == '/')
    {
        startPath = rootDir;
    }
    else
    {
        startPath = cwd;
    }

    parent = startPath;

    char* token1;
    char * saveptr = NULL;

    token1 = strtok_r(pathname, "/", &saveptr);

    if (token1 == NULL){
        if (strcmp(pathname,"/") == 0){
            ppi->parent = parent;
            ppi->index = -1;
            ppi->lastElement = NULL;
            return 0;
        }

        return -1;
    }
    
    char * token2;
    while (token1 != NULL){
        int index = findEntryInDir(parent, token1);  // look for name in directory entries one by one
        token2 = strtok_r(NULL,"/", &saveptr);
        if (token2 == NULL){
            ppi->parent = parent;
            ppi->lastElement = strdup(token1);
            ppi->index = index;

            return 0;
        }

        if (index == -1){
            return -2;
        }

        if (!isDir(&parent[index])){
            return -2;
        }

        DirEntry * temp = LoadDir (&(parent[index]));

        if (parent != startPath){
            free(parent);
        }
        
        parent = temp;
        token1 = token2;
    }

    free(startPath);
    return 0;
}

void testParsePath() {
    ppInfo *testPathInfo = malloc(sizeof(ppInfo)); // Allocate memory for path information

    // Test with the pathname "/"
    char pathname[] = "/";
    int result = parsePath(pathname, testPathInfo);

    if (result == 0) {
        // Parsing successful, print the extracted information
        printf("Parsing successful!\n");
        printf("Parent directory: %p\n", (void*)(testPathInfo->parent));
        printf("Index: %d\n", testPathInfo->index);
        printf("Last Element: %s\n", testPathInfo->lastElement);
    } else {
        printf("Error while parsing the path.\n");
    }

    free(testPathInfo); // Free the allocated memory for path information
}


int isDir(DirEntry *entry) {
    if (entry == NULL) {
        return -1; // Error code for invalid input
    }

    return entry->isDirectory; // Return directory status (1 for directory, 0 for not a directory)
}



int findEntryInDir(DirEntry *directory, char *entryName) {
    if (directory == NULL || entryName == NULL) {
        return -1; // Error code for invalid inputs
    }

    int numEntries = directory->size / sizeof(DirEntry);

    for (int i = 0; i < numEntries; i++) {
        if (strcmp(directory[i].fileName, entryName) == 0) {
            return i; // Return the index of the found directory entry
        }
    }

    return -1; // Return -1 if the entry is not found in the directory
}


DirEntry *LoadDir(DirEntry *entry) {
    if (entry == NULL || entry->isDirectory == 0) {
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
    if (blocksRead != totalBlocksToRead) {
        free(directoryStructure); // Free allocated memory in case of failure
        return NULL;
    }

    // Returning the directory structure
    return directoryStructure;
}

// Returns the index of an empty spot in Dir else returns -1
int findEmptySpotInDir(DirEntry *directory){

    int numEntries = directory->size / sizeof(DirEntry); // find the size of dir

    // loop through to find an empty spot
    for(int i = 0; i < numEntries; i++){
        if(directory[i].fileName == "\0"){
            return i;
        }
    }

    //if none found return -1
    return -1;
}
/**************************************************************
 * Class:  CSC-415-01
 * Name: Professor Bierman
 * Student ID: N/A
 * Project: Basic File System
 *
 * File: mfs.c
 *
 * Description:
 *	This is the file system interface.
 *	This is the interface needed by the driver to interact with
 *	your filesystem.
 *
 **************************************************************/

#include "mfs.h"

int fs_mkdir(const char *pathname, mode_t mode)
{
    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    if (pathInfo == NULL){
        printf("Failed allocation");
        return -1;
    }

    if (parsePath(pathname, pathInfo) < 0) { // if parsepath returns anything less than 0 
        free(pathInfo);
        return -1;
    }

    if (!isDir(pathInfo->parent)) { // if the parent of the last element isn't a directory
        printf("Parent directory does not exist or is not a directory.\n");
        free(pathInfo);
        return -1;
    }

    int lastIndex = findEntryInDir(pathInfo->parent, pathInfo->lastElement); // checking that dir
    if (lastIndex != -1) {
        printf("Directory '%s' already exists!!!!\n", pathInfo->lastElement);
        free(pathInfo);
        return -1;
    }

    // Create a new directory entry for the last element
    int emptyIndex = findEmptySpotInDir(pathInfo->parent);
    if (emptyIndex == -1) {
        printf("Parent directory is full!!!\n");
        free(pathInfo);
        return -1;
    }

    // create the new dir
    int numEntries = pathInfo->parent->size / sizeof(DirEntry);
    DirEntry *newDir = &(pathInfo->parent[emptyIndex]); 
    strcpy(pathInfo->parent[emptyIndex].fileName, pathInfo->lastElement);
    pathInfo->parent[emptyIndex].size = numEntries; 
    pathInfo->parent[emptyIndex].extentTable = allocateBlocks(numEntries, numEntries);
    pathInfo->parent[emptyIndex].lastModified = time(NULL);
    pathInfo->parent[emptyIndex].timeCreated = time(NULL);
    pathInfo->parent[emptyIndex].lastAccessed = time(NULL);
    pathInfo->parent[emptyIndex].isDirectory = 1;

    //initialize the new dir skip 0 since that contains the intiial
    for(int i = 1; i < numEntries; i ++){
        newDir[i].size = 0;
        newDir[i].extentTable = NULL;
        newDir[i].fileName[0] = '\0';
        newDir[i].lastAccessed = 0;
        newDir[i].lastModified = 0;
        newDir[i].timeCreated = 0;
        newDir[i].isDirectory = 0;
    }

    free(pathInfo);


    return 0;
}



int fs_isDir(char *pathname)
{
    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    int returnVal = parsePath(pathname, pathInfo);

    if (returnVal != 0)
    {
        free(pathInfo);
        return -1;
    }

    DirEntry *entry = pathInfo->parent;

    // if root return 1
    if (pathInfo->index == -1 || pathInfo->lastElement == NULL)
    {
        free(pathInfo);
        return 1;
    }

    DirEntry *lastElement = &(entry[pathInfo->index]); // Get the last element in the path

    int isDirectory = isDir(lastElement); // Check if it's a directory

    free(pathInfo);     // Free allocated memory for path information
    return isDirectory; // Return 1 if it's a directory, 0 if not
};

int fs_isFile(char *pathname)
{
    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    int returnVal = parsePath(pathname, pathInfo);

    if (returnVal != 0)
    {
        free(pathInfo);
        return -1;
    }

    DirEntry *entry = pathInfo->parent;

    // if root return 1
    if (pathInfo->index == -1 || pathInfo->lastElement == NULL)
    {
        free(pathInfo);
        return 0;
    }

    DirEntry *lastElement = &(entry[pathInfo->index]); // Get the last element in the path

    int isDirectory = isDir(lastElement); // Check if it's a directory

    free(pathInfo);      // Free allocated memory for path information
    return !isDirectory; // Return 1 if it's a directory, 0 if not
};

int fs_stat(const char *path, struct fs_stat *buf)
{
    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    char * pathname = strdup(path);

    int returnVal = parsePath(pathname, pathInfo);

    if (returnVal != 0)
    {
        free(pathInfo);
        return -1;
    }

    printf("INDEX: %d", pathInfo->index);
    DirEntry * entry = malloc(sizeof(DirEntry));
    
    if (pathInfo->index == -1 || pathInfo->lastElement == NULL){
        
        entry = rootDir;
        buf->st_size = 512;
        return 0;
    }

    free(pathInfo);
    free(entry);
    return 0;
}
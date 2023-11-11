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

    // checking memalloc for pathInfo
    if (pathInfo == NULL){
        printf("Failed allocation");
        return -1;
    }

    // user wrong input
    if (parsePath(pathname, pathInfo) < 0) { // if parsepath returns anything less than 0 
        free(pathInfo);
        return -1;
    }

    // user trying to make a dir in a file
    if (!isDir(pathInfo->parent)) {
        printf("Parent directory does not exist or is not a directory.\n");
        free(pathInfo);
        return -1;
    }

    // user trying to make a dir that alraeady exists
    int lastIndex = findEntryInDir(pathInfo->parent, pathInfo->lastElement); // checking that dir
    if (lastIndex != -1) {
        printf("Directory '%s' already exists!!!!\n", pathInfo->lastElement);
        free(pathInfo);
        return -1;
    }


    // finding a spot for the new dir in the parent dir
    int emptyIndex = findEmptySpotInDir(pathInfo->parent);
    if (emptyIndex == -1) {
        printf("Parent directory is full!!!\n");
        free(pathInfo);
        return -1;
    }
    // create the new dir entry
    int numEntries = pathInfo->parent->size / sizeof(DirEntry);
    DirEntry *newDir = &(pathInfo->parent[emptyIndex]); 
    strcpy(pathInfo->parent[emptyIndex].fileName, pathInfo->lastElement);
    pathInfo->parent[emptyIndex].size = numEntries; 
    pathInfo->parent[emptyIndex].extentTable = allocateBlocks(numEntries, numEntries);
    pathInfo->parent[emptyIndex].lastModified = time(NULL);
    pathInfo->parent[emptyIndex].timeCreated = time(NULL);
    pathInfo->parent[emptyIndex].lastAccessed = time(NULL);
    pathInfo->parent[emptyIndex].isDirectory = 1;
    pathInfo->parent[emptyIndex].permissions = mode; // i guess we had to have a permissions attribute to dirEntry

    // initializing the new directory
    initDirectory(newDir, globalBlockSize , pathInfo->parent);

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

// just watching buffer size
char *fs_getcwd(char *pathname, size_t size) 
{
    if (getcwd(pathname, size) == NULL) {
        perror("getcwd");
        return NULL;
    }
    return pathname;
}

int fs_setcwd(char *pathname)
{
    // need this for parsePath call
    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    // parsePath sets the CWD thanks to collin
    int result = parsePath(pathname, pathInfo);

    // check what parsepath returned
    if (result != 0) {
        free(pathInfo);  
        return -1;     
    }

    free(pathInfo);  

    return 0;  // Indicate success
}

int fs_rmdir(const char *pathname){

    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    if(parsePath(pathname, pathInfo) == -2){
        printf("\nIts not a dir!!!");
        return -1;
    }

    if(isDirEmtpy){
        DirEntry * entry = &(pathInfo->parent);
        int indexToRM = findEntryInDir(pathInfo->parent, pathInfo->lastElement);
        int result = deleteDirEntry(entry[indexToRM]);
        if(result == 0){
        printf("\n dir removed!");
        return 0;
        }else{
            printf("\n SOMETHING WENT COMPLETELY AND UTTERLY WRONG!");
            return -1;
        }
    }

}

int fs_delete(char* filename){

    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    int indexOfFile = findEntryInDir(pathInfo, filename);

    
}



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
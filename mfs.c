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

int fs_mkdir(char *pathname, mode_t mode)
{
    ppInfo *pathInfo = (ppInfo *)malloc(sizeof(ppInfo));

    int retVal = parsePath(pathname, pathInfo);

    if (retVal != 0)
    {
        printf("Parse path failed");
        return -1;
    }

    if (pathInfo->index != -1)
    {
        printf("Directory Already Exists!!");
        free(pathInfo);
        return -1;
    }

    DirEntry *parent = pathInfo->parent;

    int size = parent->size;

    printf("SIZE: %d\n", size);

    int index = findNextAvailableEntryInDir(parent);

    printf("INDEX: %d\n", index);

    if (index == -1)
    {
        printf("Error: Parent directory is full.\n");
        free(pathInfo);
        return -1;
    }

    DirEntry *dot = malloc(sizeof(DirEntry));

    int startBlock = initDirectory(20, 512, parent);

    // if (startBlock == -1)
    // {
    //     printf("Error in initDirectory()");
    //     free(pathInfo);
    //     //free(dot);
    //     return -1;
    // }

    // uint64_t blocksRead = LBAread(dot, 1, startBlock);

    // strcpy((parent[index]).fileName, pathInfo->lastElement);
    // parent[index].size = dot->size;
    // parent[index].extentTable = dot->extentTable;
    // parent[index].isDirectory = dot->isDirectory;
    // parent[index].lastAccessed = dot->lastAccessed;
    // parent[index].lastModified = dot->lastModified;
    // parent[index].timeCreated = dot->timeCreated;

    printf("FILENAME: %s", parent[index].fileName);
    free(pathInfo);
    free(dot);

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

    char *pathname = strdup(path);

    int returnVal = parsePath(pathname, pathInfo);

    if (returnVal != 0)
    {
        free(pathInfo);
        return -1;
    }

    printf("INDEX: %d", pathInfo->index);
    DirEntry *entry = malloc(sizeof(DirEntry));

    if (pathInfo->index == -1 || pathInfo->lastElement == NULL)
    {

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
    if (getcwd(pathname, size) == NULL)
    {
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
    if (result != 0)
    {
        free(pathInfo);
        return -1;
    }

    free(pathInfo);

    return 0; // Indicate success
}

// int fs_rmdir(const char *pathname){

//     ppInfo *pathInfo = malloc(sizeof(ppInfo));

//     if(parsePath(pathname, pathInfo) == -2){
//         printf("\nIts not a dir!!!");
//         return -1;
//     }

//     if(isDirEmtpy){
//         DirEntry * entry = &(pathInfo->parent);
//         int indexToRM = findEntryInDir(pathInfo->parent, pathInfo->lastElement);
//         int result = deleteDirEntry(entry[indexToRM]);
//         if(result == 0){
//         printf("\n dir removed!");
//         return 0;
//         }else{
//             printf("\n SOMETHING WENT COMPLETELY AND UTTERLY WRONG!");
//             return -1;
//         }
//     }

// }

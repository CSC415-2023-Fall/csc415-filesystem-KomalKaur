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
 *	the filesystem.
 *
 **************************************************************/

#include "mfs.h"

// Function to create a new directory with the given pathname and mode
int fs_mkdir(char *pathname, mode_t mode)
{
    printf("\nRunning mkdir...\n");
    // Allocate memory for path information
    ppInfo *pathInfo = (ppInfo *)malloc(sizeof(ppInfo));

    // Parse the given pathname and store information in pathInfo
    int retVal = parsePath(pathname, pathInfo);

    // Check if parsing was successful
    if (retVal != 0)
    {
        printf("Parse path failed");
        free(pathInfo);
        return -1;
    }

    // Check if the directory already exists
    if (pathInfo->index != -1)
    {
        printf("Directory Already Exists!!");
        free(pathInfo);
        return -1;
    }

    // Get the parent directory information
    DirEntry *parent = pathInfo->parent;

    // Get size, find an available entry, and get parent's start block
    int size = parent->size;
    int index = findNextAvailableEntryInDir(parent);
    int parentStartBlock = parent->directoryStartBlock;

    // Check if the parent directory is full
    if (index == -1)
    {
        printf("Error: Parent directory is full.\n");
        free(pathInfo);
        return -1;
    }

    // Allocate memory for a new directory entry
    DirEntry *dot = malloc(sizeof(DirEntry));

    // Initialize a new directory and get its start block
    int startBlock = initDirectory(20, 512, parent);
    printf("START BLOCK IN MKDIR(): %d\n", startBlock);

    // Check for initialization errors
    if (startBlock == -1)
    {
        printf("Error in initDirectory()");
        free(pathInfo);
        free(dot);
        return -1;
    }

    // Read the directory entry from the start block
    uint64_t blocksRead = LBAread(dot, 1, startBlock);

    // Copy information to the parent directory
    strcpy((parent[index]).fileName, pathInfo->lastElement);
    parent[index].size = dot->size;
    parent[index].extentTable = dot->extentTable;
    parent[index].isDirectory = dot->isDirectory;
    parent[index].lastAccessed = dot->lastAccessed;
    parent[index].lastModified = dot->lastModified;
    parent[index].timeCreated = dot->timeCreated;
    parent[index].directoryStartBlock = dot->directoryStartBlock;

    // Calculate the number of blocks needed
    int bSize = 512;
    int numBlocks = (dot->size + bSize - 1) / bSize;

    // Write the updated parent directory back to disk
    LBAwrite(parent, numBlocks, parentStartBlock);

    // Free allocated memory
    free(pathInfo);
    free(dot);

    // Return success
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

// Function to retrieve file or directory information
int fs_stat(const char *path, struct fs_stat *buf)
{
    // Allocate memory for path information
    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    // Duplicate the path to avoid modifying the original
    char *pathname = strdup(path);

    // Parse the given path and store information in pathInfo
    int returnVal = parsePath(pathname, pathInfo);

    // Check for parsing errors
    if (returnVal != 0)
    {
        free(pathInfo);
        return -1;
    }

    // Get the parent directory information
    DirEntry *parent = pathInfo->parent;

    // Find the index of the entry in the parent directory
    int index = findEntryInDir(parent, pathInfo->lastElement);

    // Check if the path does not exist or is invalid
    if (pathInfo->index == -1 || pathInfo->lastElement == NULL)
    {   
        // Calculate size in blocks
        int bSize = 512;
        int sizeInBlocks = (parent[0].size + bSize - 1) / bSize;

        // Set information in buf for the parent directory
        buf->st_accesstime = parent[0].lastAccessed;
        buf->st_createtime = parent[0].timeCreated;
        buf->st_modtime = parent[0].lastModified;
        buf->st_size = parent[0].size;
        buf->st_blocks = sizeInBlocks;

        // Return success
        return 0;
    }

    // Calculate size in blocks for the specified entry
    int bSize = 512;
    int sizeInBlocks = (parent[index].size + bSize - 1) / bSize;

    // Set information in buf for the specified entry
    buf->st_accesstime = parent[index].lastAccessed;
    buf->st_createtime = parent[index].timeCreated;
    buf->st_modtime = parent[index].lastModified;
    buf->st_size = parent[index].size;
    buf->st_blocks = sizeInBlocks;

    // Free allocated memory
    free(pathInfo);

    // Return success
    return 0;
}

// TODO: fix the last two functions in here


// Function to get the current working directory and store it in the provided buffer
// - just watching buffer size
char *fs_getcwd(char *pathname, size_t size)
{
    // Use getcwd to retrieve the current working directory into the provided buffer
    if (getcwd(pathname, size) == NULL)
    {
        // If getcwd fails, print an error message and return NULL
        perror("getcwd");
        return NULL;
    }

    // Return the updated pathname buffer
    return pathname;
}

// Function to set the current working directory based on the provided pathname
int fs_setcwd(char *pathname)
{
    // Allocate memory for path information
    // - need this for parsePath call
    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    // Call parsePath to set the current working directory based on the provided pathname
    int result = parsePath(pathname, pathInfo);

    // Check the result of parsePath
    if (result != 0)
    {
        // If parsePath fails, free allocated memory and return -1
        free(pathInfo);
        return -1;
    }

    // Free allocated memory after successfully setting the current working directory
    free(pathInfo);

    // Indicate success by returning 0
    return 0;
}

/*
Returns fdDir --> 
    unsigned short  d_reclen    length of this record 
	unsigned short	dirEntryPosition;	which directory entry position, like file pos 
	DE *	directory;			Pointer to the loaded directory you want to iterate 
	struct fs_diriteminfo * di;

fs_diriteminfo --> 
    unsigned short d_reclen;    length of this record 
    unsigned char fileType;    
    char d_name[256]; 
*/
fdDir * fs_opendir(const char *pathname) 
{
    printf("\nRunning fs_opendir...\n");

    ppInfo *pathInfo = malloc(sizeof(ppInfo));

    if (pathInfo == NULL)
    {
        printf("Error allocating memory for pathInfo\n");
        free(pathInfo);
        return NULL;
    }

    char path[strlen(pathname)];
    strcpy(path, pathname);
    printf("path is %s\n", path);

    int retVal = parsePath(path, pathInfo);

    if (retVal != 0)
    {
        printf("Error occured with the parsePath!\n");
        free(pathInfo);
        return NULL;
    }

    fdDir * dirp = malloc(sizeof(fdDir));
    
    if (dirp == NULL)
    {
        printf("Error allocating memory for fdDir\n");
        free(dirp);
        return NULL;
    }

    dirp->dirEntryPosition = 0;
    dirp->directory = pathInfo->parent;
    dirp->d_reclen = dirp->directory->size; 
    dirp->di = NULL; /* Pointer to the structure you return from read */

    printf("~Path Info~\n");
    printf("Index: %d\n", pathInfo->index);

    printf("\n~dirp information~\n");
    printf("dirEntryPosition: %d\n", dirp->dirEntryPosition);
    //printf("directory: %s\n", dirp->directory);
    printf("length:%d\n", dirp->d_reclen);
    return dirp;       
}

// 
struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{
    printf("\nRunning readdir...\n");
    if (dirp == NULL)
    {
        printf("Error: Nothing to read.\n");
        return NULL;
    }

    struct fs_diriteminfo * iteminfo = malloc(sizeof(struct fs_diriteminfo));

    if (iteminfo == NULL)
    {
        printf("Error allocating memory for iteminfo\n");
        free(iteminfo);
        return NULL;
    }

    return iteminfo;

}

int fs_closedir(fdDir *dirp)
{
    printf("\nRunning closedir...\n");
    if (dirp != NULL)
    {
        free(dirp->di);
        free(dirp);  
        return 0;
    }
    return -1;
}
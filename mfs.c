/**************************************************************
 * Class:  CSC-415-01
 * Name: Professor Bierman
 * Student ID: N/A
 * Project: Basic File System
 *
 * File: mfs.c
 *
 * Description:
 * This is the implementation of the functions needed by the
 * driver to interact with the filesystem.
 *
 **************************************************************/

#include "mfs.h"

// Function to create a new directory with the given pathname and mode
int fs_mkdir(char *pathname, mode_t mode)
{
    // Allocate memory for path information
    ppInfo *pathInfo = (ppInfo *)malloc(sizeof(ppInfo));

    // Parse the given pathname and store information in pathInfo
    int retVal = parsePath(pathname, pathInfo);

    // Check if parsing was successful
    if (retVal != 0)
    {
        printf("Parse path failed\n");
        free(pathInfo);
        return -1;
    }

    // Check if the directory already exists
    if (pathInfo->index != -1)
    {
        printf("Directory Already Exists!!\n");
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
    
    // Check for initialization errors
    if (startBlock == -1)
    {
        printf("Error in initDirectory()\n");
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
    parent[index].directoryBlockCount = dot->directoryBlockCount;
    

    // Calculate the number of blocks needed
    int bSize = 512;
    int numBlocks = (dot->size + bSize - 1) / bSize;

    // Write the updated parent directory back to disk
    LBAwrite(parent, numBlocks, parentStartBlock);

    fs_setcwd(pathname);

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

int fs_rmdir(char *pathname) {
    // Allocate memory for path information
    ppInfo *pathInfo = (ppInfo *)malloc(sizeof(ppInfo));

    // Parse the given pathname and store information in pathInfo
    int retVal = parsePath(pathname, pathInfo);

    // Check if parsing was successful
    if (retVal != 0) {
        printf("Parse path failed\n");
        free(pathInfo);
        return -1;
    }

    DirEntry *parent = pathInfo->parent;
    int index = pathInfo->index;

    // Check if the directory exists
    if (pathInfo->index == -1 || pathInfo->lastElement == NULL) {
        printf("Directory not found\n");
        free(pathInfo);
        return -1;
    }


    // Check if the entry to remove is a directory
    if (!isDir(&parent[index])) {
        printf("Not a directory\n");
        free(pathInfo);
        return -1;
    }   

    strcpy(parent[index].fileName, "-1");
    parent[index].extentTable = NULL;

    // Calculate the number of blocks used by the directory
    int bSize = 512;
    int numBlocks = (parent[index].size + bSize - 1) / bSize;

    // Set the corresponding bits in the free space map as free
    freeBlocks(parent[index].directoryStartBlock, numBlocks);

    // Update the parent directory on disk
    int parentStartBlock = pathInfo->parent->directoryStartBlock;

    int parentCount = pathInfo->parent->directoryBlockCount;

    LBAwrite(pathInfo->parent, parentCount, parentStartBlock);

    // Free allocated memory
    free(pathInfo);
    return 0;
}

int fs_delete(char* filename) {
    // Allocate memory for path information
    ppInfo *pathInfo = (ppInfo *)malloc(sizeof(ppInfo));

    // Parse the given filename and store information in pathInfo
    int retVal = parsePath(filename, pathInfo);

    // Check if parsing was successful
    if (retVal != 0) {
        printf("Parse path failed\n");
        free(pathInfo);
        return -1;
    }

    DirEntry *parent = pathInfo->parent;
    int index = pathInfo->index;

    // Check if the file exists
    if (pathInfo->index == -1 || pathInfo->lastElement == NULL) {
        printf("File not found\n");
        free(pathInfo);
        return -1;
    }

    // Check if the entry to remove is a file
    if (isDir(&parent[index])) {
        printf("Not a file\n");
        free(pathInfo);
        return -1;
    }

    // Mark the entry as available
    strcpy(parent[index].fileName, "-1");
    parent[index].extentTable = NULL;

    // Calculate the number of blocks used by the file
    int bSize = 512;
    int numBlocks = (parent[index].size + bSize - 1) / bSize;

    // Set the corresponding bits in the free space map as free
    freeBlocks(parent[index].directoryStartBlock, numBlocks);

    // Update the parent directory on disk
    int parentStartBlock = pathInfo->parent->directoryStartBlock;

    int parentCount = pathInfo->parent->directoryBlockCount;

    LBAwrite(pathInfo->parent, parentCount, parentStartBlock);

    // Free allocated memory
    free(pathInfo);
    return 0;
}

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
    if (cwd == NULL){
        pathname == NULL;
    }

    pathname = cwd->fileName;

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

    DirEntry *parent = pathInfo->parent;
    cwd = malloc(sizeof(DirEntry));

    if (pathInfo->index == -1 || pathInfo->lastElement == NULL)
    {
        cwd = rootDir;
    }
    else
    {
        int index = findEntryInDir(parent, pathInfo->lastElement);
        DirEntry *temp = &parent[index];
        cwd = temp;
    }

    // Free allocated memory after successfully setting the current working directory
    free(pathInfo);
    // Indicate success by returning 0
    return 0;
}


/*
Returns fdDir --> 
    unsigned short  d_reclen    length of this record 
	unsigned short	dirEntryPosition;	which directory entry position
	DE *	directory;			Pointer to the loaded directory we iterate 
	struct fs_diriteminfo * di;

fs_diriteminfo --> 
    unsigned short d_reclen;    length of this record 
    unsigned char fileType;    
    char d_name[256]; 
*/
fdDir *fs_opendir(const char *pathname)
{
    // Allocate memory for directory info structure
    fdDir *dirp = (fdDir *)malloc(sizeof(fdDir));

    if (dirp == NULL)
    {
        perror("Error in allocating memory for directory info");
        return NULL;
    }

    // Initialize directory info structure
    dirp->d_reclen = sizeof(struct fs_diriteminfo);
    dirp->dirEntryPosition = 0;
    dirp->di = NULL; // No directory entry info to start with

    // Allocate memory for path information
    ppInfo *pathInfo = (ppInfo *)malloc(sizeof(ppInfo));

    if (pathInfo == NULL)
    {
        perror("Error in allocating memory for path information");
        free(dirp); // Free allocated memory for directory info structure
        return NULL;
    }

    // Parse the given pathname and store information in pathInfo
    char path[strlen(pathname)];
    strcpy(path, pathname);
    int returnVal = parsePath(path, pathInfo);

    // Check if parsing was successful
    if (returnVal != 0)
    {
        printf("Parse path failed");
        free(dirp);
        free(pathInfo);
        return NULL;
    }

    // Check if the path corresponds to a directory
    if (pathInfo->index == -1 || pathInfo->lastElement == NULL || !isDir(&(pathInfo->parent[0])))
    {
        printf("Error: Not a directory");
        free(dirp);
        free(pathInfo);
        return NULL;
    }

    // Load the directory structure based on the parsed path
    DirEntry *loadedDir = LoadDir(&(pathInfo->parent[0]));

    if (loadedDir == NULL)
    {
        printf("Error loading directory");
        free(dirp);
        free(pathInfo);
        return NULL;
    }

    dirp->directory = loadedDir;
    // Set the directory entry information in the directory info structure
    dirp->di = (struct fs_diriteminfo *)malloc(sizeof(struct fs_diriteminfo));
    dirp->di->d_reclen = sizeof(struct fs_diriteminfo);
    dirp->di->fileType = 1; // Assuming it's always a directory
    strcpy(dirp->di->d_name, pathInfo->lastElement);

    // Free allocated memory for path information
    free(pathInfo);

    printf("~Path Info~\n");
    printf("Index: %d\n", pathInfo->index);

    printf("\n~dirp information~\n");
    printf("dirEntryPosition: %d\n", dirp->dirEntryPosition);
    printf("directory: %s\n", dirp->directory->fileName);
    printf("length:%d\n", dirp->d_reclen);
    return dirp;       
}

/* 
Returns a pointer to a DirEntry structure representing the 
    next directory entry in the directory stream pointed to by dirp
Returns NULL on reaching the end of the directory stream or if an error occurred.
*/
// struct fs_diriteminfo *fs_readdir(fdDir *dirp)
// {
//     // Check if the directory info structure is valid
//     if (dirp == NULL || dirp->di == NULL)
//     {
//         printf("Invalid directory info structure");
//         return NULL;
//     }

//     // Check if the directory is empty or if all entries have been read
//     if (dirp->dirEntryPosition >= (dirp->parent[dirp->index]).size / sizeof(DirEntry))
//     {
//         // All entries have been read
//         return NULL;
//     }

//     // Get the current directory entry position
//     int currentPos = dirp->dirEntryPosition;

//     // Increment the directory entry position for the next call
//     (dirp->dirEntryPosition)++;

//     // Get the current directory entry
//     DirEntry *currentEntry = &(dirp->parent[dirp->index]);

//     // Move to the next directory entry
//     currentEntry = &(currentEntry[currentPos]);

//     // Allocate memory for directory entry info structure
//     struct fs_diriteminfo *nextDir = (struct fs_diriteminfo *)malloc(sizeof(struct fs_diriteminfo));

//     if (nextDir == NULL)
//     {
//         perror("Error in allocating memory for directory entry info");
//         return NULL;
//     }

//     // Set the directory entry info structure
//     nextDir->d_reclen = sizeof(struct fs_diriteminfo);
//     nextDir->fileType = currentEntry->isDirectory; // Assuming isDirectory is set correctly
//     strcpy(nextDir->d_name, currentEntry->fileName);

//     return nextDir;
// }

int fs_closedir(fdDir *dirp)
{
    printf("\nRunning closedir...\n");

    // Check if the directory info structure is valid
    if (dirp == NULL)
    {
        printf("Invalid directory info structure");
        return -1;
    }

    // Free the memory allocated for the directory info structure
    free(dirp->di);
    free(dirp);

    // Indicate success
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
// fdDir * fs_opendir(const char *pathname) 
// {
//     printf("\nRunning fs_opendir...\n");

//     ppInfo *pathInfo = malloc(sizeof(ppInfo));

//     if (pathInfo == NULL)
//     {
//         printf("Error allocating memory for pathInfo\n");
//         free(pathInfo);
//         return NULL;
//     }

//     char path[strlen(pathname)];
//     strcpy(path, pathname);
//     printf("path is %s\n", path);

//     int retVal = parsePath(path, pathInfo);

//     if (retVal != 0)
//     {
//         printf("Error occured with the parsePath!\n");
//         free(pathInfo);
//         return NULL;
//     }

//     fdDir * dirp = malloc(sizeof(fdDir));
    
//     if (dirp == NULL)
//     {
//         printf("Error allocating memory for fdDir\n");
//         free(dirp);
//         return NULL;
//     }

//     dirp->dirEntryPosition = 0;
//     dirp->directory = pathInfo->parent;
//     dirp->d_reclen = dirp->directory->size; 
//     dirp->di = NULL; /* Pointer to the structure you return from read */

//     printf("~Path Info~\n");
//     printf("Index: %d\n", pathInfo->index);

//     printf("\n~dirp information~\n");
//     printf("dirEntryPosition: %d\n", dirp->dirEntryPosition);
//     //printf("directory: %s\n", dirp->directory);
//     printf("length:%d\n", dirp->d_reclen);
//     return dirp;       
// }

// // 
// struct fs_diriteminfo *fs_readdir(fdDir *dirp)
// {
//     printf("\nRunning readdir...\n");
//     if (dirp == NULL)
//     {
//         printf("Error: Nothing to read.\n");
//         return NULL;
//     }

//     struct fs_diriteminfo * iteminfo = malloc(sizeof(struct fs_diriteminfo));

//     if (iteminfo == NULL)
//     {
//         printf("Error allocating memory for iteminfo\n");
//         free(iteminfo);
//         return NULL;
//     }

//     return iteminfo;

// }

// int fs_closedir(fdDir *dirp)
// {
//     printf("\nRunning closedir...\n");
//     if (dirp != NULL)
//     {
//         free(dirp->di);
//         free(dirp);  
//         return 0;
//     }
//     return -1;
// }
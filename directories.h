#ifndef DIRECTORIES_H
#define DIRECTORIES_H

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "freespace.h"
#include "b_io.h"

#define MAX_FILE_NAME 256

typedef struct {
    char fileName[MAX_FILE_NAME];
    unsigned long size;
    unsigned int fileLocation;
    time_t lastModified;
    time_t lastAccessed;
    time_t timeCreated;
    char isDirectory;
} DirEntry;

int initDirectory(int initialDirEntries, uint64_t numberOfBlocks, uint64_t blockSize);

#endif

#ifndef DIRECTORIES_H
#define DIRECTORIES_H

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "freespace.h"
#include "b_io.h"
#include "fsLow.h"

#define MAX_FILE_NAME 256

typedef struct {
    char fileName[MAX_FILE_NAME];
    unsigned long size;
    extent * extentTable;
    time_t lastModified;
    time_t lastAccessed;
    time_t timeCreated;
    char isDirectory;
} DirEntry;

int initDirectory(int initialDirEntries, uint64_t blockSize, DirEntry *parent);

#endif

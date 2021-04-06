#ifndef FS_H
#define FS_H

#include <scetypes.h>

const char *getFileExt(const char *filename);
int writeFile(char *path, void *buf, int size);
int readFilemp(const char *path, void *buf, int size);
int getFileSize(const char *path, SceOff *size);
int getFileCreatedAt(const char *path, SceDateTime *ctime);
SceBool fileExists(const char *path);
SceBool dirExists(const char *path);

#endif
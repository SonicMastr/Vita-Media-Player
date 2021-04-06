#include <string.h>

#include "common.h"
#include "fs.h"


const char *getFileExt(const char *filename) {
	const char *dot = strrchr(filename, '.');
	
	if (!dot || dot == filename)
		return "";
	
	return dot + 1;
}

int writeFile(char *path, void *buf, int size) {
	SceUID file = 0;
	
	if (R_SUCCEEDED(file = sceIoOpen(path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777))) {
		int bytes_written = sceIoWrite(file, buf, size);
		sceIoClose(file);
		return bytes_written;
	}

	return file;
}

int readFilemp(const char *path, void *buf, int size) {
	SceUID file = 0;

	if (R_SUCCEEDED(file = sceIoOpen(path, SCE_O_RDONLY, 0))) {
		int bytes_read = sceIoRead(file, buf, size);
		sceIoClose(file);
		return bytes_read;
	}
	
	return file;
}

SceBool dirExists(const char *path) {
	SceUID dir = 0;
	
	if (R_SUCCEEDED(dir = sceIoDopen(path))) {
		sceIoDclose(dir);
		return SCE_TRUE;
	}
	
	return SCE_FALSE;
}

SceBool fileExists(const char *path) {
	SceUID file = 0;
	
	if (R_SUCCEEDED(file = sceIoOpen(path, SCE_O_RDONLY, 0777))) {
		sceIoClose(file);
		return SCE_TRUE;
	}
	
	return SCE_FALSE;
}

int getFileSize(const char *path, SceOff *size) {
	SceIoStat stat;
	int ret = 0;

	if (R_FAILED(ret = sceIoGetstat(path, &stat)))
		return ret;

	*size = stat.st_size;

	return 0;
}

int getFileCreatedAt(const char *path, SceDateTime *ctime) {
	SceIoStat stat;
	int ret = 0;

	if (R_FAILED(ret = sceIoGetstat(path, &ctime)))
		return ret;
	
	*ctime = stat.st_ctime;

	return 0;
}
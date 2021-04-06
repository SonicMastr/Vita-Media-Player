#ifndef DIR_H
#define DIR_H

#include <scetypes.h>

typedef struct File {
	struct File *next; // Next item
	SceBool is_dir;        // Folder flag
	char name[256];    // File name
	char ext[5];       // File extension
} File;

extern File *files;

int getDirListing(SceBool refresh);
void displayFiles();
void openFile(void);
int navigate(SceBool parent);
int getLastDirectory(void);
int handleDirControls(void);

#endif
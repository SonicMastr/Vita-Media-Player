/* All Credits for this goes to Joel16 on github and his ElevenMPV app */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "avplayer.h"
#include "common.h"
#include "dir.h"
#include "utils.h"
#include "fs.h"
#include "texture.h"

#define 	SCE_S_ISDIR(m)   (((m) & SCE_S_IFMT) == SCE_S_IFDIR)

int config = 0;

File *files = NULL;


static void dirListFree(File *node) {
	if (node == NULL) // End of list
		return;
	
	dirListFree(node->next); // Nest further
	free(node); // Free memory
}

static void saveLastDirectory(void) {
	char *buf = malloc(256);
	int len = snprintf(buf, 256, "%s\n", curDir);
	writeFile("ux0:data/SubPlayer/lastdir.txt", buf, len);
	free(buf);
}

static int cmpstringp(const void *p1, const void *p2) {
	SceIoDirent *entryA = (SceIoDirent *)p1;
	SceIoDirent *entryB = (SceIoDirent *)p2;

	if ((SCE_STM_ISDIR(entryA->d_stat.st_mode)) && !(SCE_STM_ISDIR(entryB->d_stat.st_mode)))
		return -1;
	else if (!(SCE_STM_ISDIR(entryA->d_stat.st_mode)) && (SCE_STM_ISDIR(entryB->d_stat.st_mode)))
		return 1;
	else {
		if (config == 0) // Sort alphabetically (ascending - A to Z)
			return strcasecmp(entryA->d_name, entryB->d_name);
		else if (config == 1) // Sort alphabetically (descending - Z to A)
			return strcasecmp(entryB->d_name, entryA->d_name);
		else if (config == 2) // Sort by file size (largest first)
			return entryA->d_stat.st_size > entryB->d_stat.st_size ? -1 : entryA->d_stat.st_size < entryB->d_stat.st_size ? 1 : 0;
		else if (config == 3) // Sort by file size (smallest first)
			return entryB->d_stat.st_size > entryA->d_stat.st_size ? -1 : entryB->d_stat.st_size < entryA->d_stat.st_size ? 1 : 0;
	}

	return 0;
}

int getDirListing(SceBool refresh)
{
    SceUID dir = 0;
    dirListFree(files);
	files = NULL;
	file_count = 0;

    SceBool parent_dir_set = SCE_FALSE;
    if (R_SUCCEEDED(dir = sceIoDopen(curDir))) {
        int count = 0;
        SceIoDirent *entries = (SceIoDirent *)calloc(MAX_FILES, sizeof(SceIoDirent));
        while (sceIoDread(dir, &entries[count]) > 0)
			count++;
        sceIoDclose(dir);
		printf("Closed Directory\n");
		qsort(entries, count, sizeof(SceIoDirent), cmpstringp);
		printf("Sorted\n");
        for (int i = -1; i < count; i++) {
			// Allocate Memory
			File *item = (File *)malloc(sizeof(File));
			memset(item, 0, sizeof(File));
			printf("Allocated Memory\n");
			if ((strcmp(curDir, root_path)) && (i == -1) && (!parent_dir_set)) {
				strcpy(item->name, "..");
				item->is_dir = SCE_TRUE;
				parent_dir_set = SCE_TRUE;
				file_count++;
			}
			else {
				if ((i == -1) && (!(strcmp(curDir, root_path))))
					continue;

				item->is_dir = SCE_STM_ISDIR(entries[i].d_stat.st_mode);

				// Copy File Name
				strcpy(item->name, entries[i].d_name);
				strcpy(item->ext, getFileExt(item->name));
				file_count++;
			}

			// New List
			if (files == NULL) 
				files = item;

			// Existing List
			else {
				File *list = files;
					
				while(list->next != NULL) 
					list = list->next;

				list->next = item;
			}
		}

    }
    else
		return dir;
    
    if (!refresh) {
        if (position >= file_count)
            position = file_count - 1;
    }
    else
        position = 0;

    return 0;
}

void displayFiles() {
	int i = 0, printed = 0;
	File *file = files; // Draw file list
	int startXScale = (int)(FRAMEBUF_WIDTH*.0520f);
	float textureScale = (FRAMEBUF_WIDTH*.2f)/200.0f;
	float basePgfScale = FRAMEBUF_HEIGHT/544; // 544 is the perfect size for a scale of 1, and the default anyway
	for(; file != NULL; file = file->next) {
		if (printed == FILES_PER_PAGE)
			break;
		if (position < FILES_PER_PAGE || i > (position - FILES_PER_PAGE)) {
			if (i == position) {
				vita2d_draw_rectangle(0, (FRAMEBUF_HEIGHT*ENTRY_SCALE*printed), FRAMEBUF_WIDTH*0.6f, FRAMEBUF_HEIGHT*ENTRY_SCALE, RGBA8(120, 120, 255, 200));
				if (file->is_dir)
					vita2d_draw_texture_scale(folder, FRAMEBUF_WIDTH - (FRAMEBUF_WIDTH*.3f), FRAMEBUF_HEIGHT*.1f, textureScale, textureScale);
				else if ((!strncasecmp(file->ext, "flac", 4)) || (!strncasecmp(file->ext, "it", 4)) || (!strncasecmp(file->ext, "mod", 4))
					|| (!strncasecmp(file->ext, "mp3", 4)) || (!strncasecmp(file->ext, "ogg", 4)) || (!strncasecmp(file->ext, "opus", 4))
					|| (!strncasecmp(file->ext, "s3m", 4))|| (!strncasecmp(file->ext, "wav", 4)) || (!strncasecmp(file->ext, "xm", 4)))
						vita2d_draw_texture_scale(music, FRAMEBUF_WIDTH - (FRAMEBUF_WIDTH*.3f), FRAMEBUF_HEIGHT*.1f, textureScale, textureScale);
				else if ((!strncasecmp(file->ext, "mp4", 4)))
					vita2d_draw_texture_scale(video, FRAMEBUF_WIDTH - (FRAMEBUF_WIDTH*.3f), FRAMEBUF_HEIGHT*.1f, textureScale, textureScale);
				int textWidth = vita2d_pgf_text_width(pgf, basePgfScale, file->name);
				int x = (FRAMEBUF_WIDTH*.8f) - (textWidth/2.0f);
				if (FRAMEBUF_WIDTH*.65f > ((FRAMEBUF_WIDTH*.8f)-(textWidth/2.0f)))
					x = FRAMEBUF_WIDTH*.66f;
				vita2d_pgf_draw_text(pgf, x, (FRAMEBUF_HEIGHT*ENTRY_SCALE/2.0f) + (FRAMEBUF_HEIGHT*.5f), RGBA8(255, 255, 255, 255), basePgfScale, file->name);
			}

			if (strncmp(file->name, "..", 2) == 0)
				vita2d_pgf_draw_text(pgf, startXScale, (FRAMEBUF_HEIGHT*ENTRY_SCALE/2.0f) + (FRAMEBUF_HEIGHT*ENTRY_SCALE*(float)printed), RGBA8(255, 255, 255, 255), basePgfScale*.9f, "Parent folder");
			else
				vita2d_pgf_draw_text(pgf, startXScale, (FRAMEBUF_HEIGHT*ENTRY_SCALE/2.0f) + (FRAMEBUF_HEIGHT*ENTRY_SCALE*(float)printed), RGBA8(255, 255, 255, 255), basePgfScale*.9f, file->name);

			printed++;
		}

		i++;
	}
	vita2d_draw_line(FRAMEBUF_WIDTH*0.6f,0,FRAMEBUF_WIDTH*0.6f,FRAMEBUF_HEIGHT, RGBA8(255, 255, 255, 255));
}

File *getFileIndex(int index) {
	int i = 0;
	File *file = files; // Find file Item
	
	for(; file != NULL && i != index; file = file->next)
		i++;

	return file; // Return file
}

int navigate(SceBool parent) {
	File *file = getFileIndex(position); // Get index
	
	if (file == NULL)
		return -1;

	// Special case ".."
	if ((parent) || (!strncmp(file->name, "..", 2))) {
		char *slash = NULL;

		// Find last '/' in working directory
		int i = strlen(curDir) - 2; for(; i >= 0; i--) {
			// Slash discovered
			if (curDir[i] == '/') {
				slash = curDir + i + 1; // Save pointer
				break; // Stop search
			}
		}

		slash[0] = 0; // Terminate working directory
	}

	// Normal folder
	else {
		if (file->is_dir) {
			// Append folder to working directory
			strcpy(curDir + strlen(curDir), file->name);
			curDir[strlen(curDir) + 1] = 0;
			curDir[strlen(curDir)] = '/';
		}
	}

	saveLastDirectory();

	return 0; // Return success
}

void openFile(void) {
	char path[512];
	File *file = getFileIndex(position);

	if (file == NULL)
		return;

	strcpy(path, curDir);
	strcpy(path + strlen(path), file->name);

	if (file->is_dir) {
		// Attempt to navigate to target
		if (R_SUCCEEDED(navigate(SCE_FALSE))) {
			getDirListing(SCE_TRUE);
		}
	}
	else if ((!strncasecmp(file->ext, "mp4", 4))) {
		startPlayback(path);
    }
		//Menu_PlayAudio(path);
}

int getLastDirectory(void) {
	int ret = 0;
	const char *root_paths[] = {
		"ux0:/",
		"ur0:/",
		"uma0:/"
	};
	
	if (!fileExists("ux0:data/SubPlayer/lastdir.txt")) {
		snprintf(root_path, 8, "ux0:/");
		writeFile("ux0:data/SubPlayer/lastdir.txt", root_path, strlen(root_path) + 1);
		strcpy(curDir, root_path); // Set Start Path to "sdmc:/" if lastDir.txt hasn't been created.
	}
	else {
		strcpy(root_path, root_paths[0]);
		SceOff size = 0;

		getFileSize("ux0:data/SubPlayer/lastdir.txt", &size);
		char *buf = malloc(size + 1);

		if (R_FAILED(ret = readFilemp("ux0:data/SubPlayer/lastdir.txt", buf, size))) {
			free(buf);
			return ret;
		}

		buf[size] = '\0';
		char path[512];
		sscanf(buf, "%[^\n]s", path);
	
		if (dirExists(path)) // Incase a directory previously visited had been deleted, set start path to sdmc:/ to avoid errors.
			strcpy(curDir, path);
		else
			strcpy(curDir, root_path);
		
		free(buf);
	}
	
	return 0;
}

int handleDirControls()
{
	if (file_count > 0) {
		if (pressed & SCE_CTRL_UP)
			position--;
		else if (pressed & SCE_CTRL_DOWN)
			position++;

		setMax(&position, 0, file_count - 1);
		setMin(&position, file_count - 1, 0);

		if (pressed & SCE_CTRL_LEFT)
			position = 0;
		else if (pressed & SCE_CTRL_RIGHT)
			position = file_count - 1;

		if (pressed & SCE_CTRL_CROSS)
			openFile();
	}

	if ((strcmp(curDir, root_path) != 0) && (pressed & SCE_CTRL_CANCEL)) {
		navigate(SCE_TRUE);
		getDirListing(SCE_TRUE);
	}
    return 0;
}


#include "common.h"

vita2d_pvf *subtitleFont;
SceUInt32 SCE_CTRL_ENTER;
SceUInt32 SCE_CTRL_CANCEL;
SceUInt32 pressed = 0;
void *mspaceGlobal;
vita2d_pgf *pgf;
int position = 0;
int file_count = 0;
char curDir[512] = "app0:";
char root_path[8];
PlayerInitStatus playerStatus;
PlayerState playerState;
ThumbnailStatus thumbnailStatus;
uint64_t streamDuration = 0;
SubtitlesStatus subStatus;
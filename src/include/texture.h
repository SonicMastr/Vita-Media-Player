#ifndef TEXTURE_H
#define TEXTURE_H

#include <vita2d_sys.h>

vita2d_texture *play, *pause, *stop, *backwards, *forwards, *subtitles, *subtitles_disabled, *video, *music, *folder, *loading;

void loadTextures(void);
void freeTextures(void);

#endif

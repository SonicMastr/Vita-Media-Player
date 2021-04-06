#include <scetypes.h>

#include "texture.h"

extern SceUChar8 _binary_resources_play_png_start;
extern SceUChar8 _binary_resources_pause_png_start;
extern SceUChar8 _binary_resources_stop_png_start;
extern SceUChar8 _binary_resources_backwards_png_start;
extern SceUChar8 _binary_resources_forwards_png_start;
extern SceUChar8 _binary_resources_subtitles_png_start;
extern SceUChar8 _binary_resources_subtitlesd_png_start;
extern SceUChar8 _binary_resources_video_png_start;
extern SceUChar8 _binary_resources_music_png_start;
extern SceUChar8 _binary_resources_folder_png_start;
extern SceUChar8 _binary_resources_loading_png_start;
extern SceUChar8 _binary_resources_play_png_size;
extern SceUChar8 _binary_resources_pause_png_size;
extern SceUChar8 _binary_resources_stop_png_size;
extern SceUChar8 _binary_resources_backwards_png_size;
extern SceUChar8 _binary_resources_forwards_png_size;
extern SceUChar8 _binary_resources_subtitles_png_size;
extern SceUChar8 _binary_resources_subtitlesd_png_size;
extern SceUChar8 _binary_resources_video_png_size;
extern SceUChar8 _binary_resources_music_png_size;
extern SceUChar8 _binary_resources_folder_png_size;
extern SceUChar8 _binary_resources_loading_png_size;


static vita2d_texture *Texture_LoadImageBilinear(ScePVoid buffer, unsigned long size) {
	vita2d_texture *texture = vita2d_load_PNG_buffer(buffer, size);
	vita2d_texture_set_filters(texture, SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR);
	return texture;
}

void loadTextures() {
	play = Texture_LoadImageBilinear(&_binary_resources_play_png_start, &_binary_resources_play_png_size);
    pause = Texture_LoadImageBilinear(&_binary_resources_pause_png_start, &_binary_resources_pause_png_size);
    stop = Texture_LoadImageBilinear(&_binary_resources_stop_png_start, &_binary_resources_stop_png_size);
    backwards = Texture_LoadImageBilinear(&_binary_resources_backwards_png_start, &_binary_resources_backwards_png_size);
    forwards = Texture_LoadImageBilinear(&_binary_resources_forwards_png_start, &_binary_resources_forwards_png_size);
    subtitles = Texture_LoadImageBilinear(&_binary_resources_subtitles_png_start, &_binary_resources_subtitles_png_size);
    subtitles_disabled = Texture_LoadImageBilinear(&_binary_resources_subtitlesd_png_start, &_binary_resources_subtitlesd_png_size);
    video = Texture_LoadImageBilinear(&_binary_resources_video_png_start, &_binary_resources_video_png_size);
    music = Texture_LoadImageBilinear(&_binary_resources_music_png_start, &_binary_resources_music_png_size);
    folder = Texture_LoadImageBilinear(&_binary_resources_folder_png_start, &_binary_resources_folder_png_size);
    loading = Texture_LoadImageBilinear(&_binary_resources_loading_png_start, &_binary_resources_loading_png_size);
}

void freeTextures() {
	vita2d_free_texture(play);
    vita2d_free_texture(pause);
    vita2d_free_texture(stop);
    vita2d_free_texture(backwards);
    vita2d_free_texture(forwards);
    vita2d_free_texture(subtitles);
    vita2d_free_texture(subtitles_disabled);
    vita2d_free_texture(video);
    vita2d_free_texture(music);
    vita2d_free_texture(folder);
    vita2d_free_texture(loading);
}

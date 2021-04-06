#ifndef COMMON_H
#define COMMON_H

#include <ctrl.h>
#include <scetypes.h>
#include <vita2d_sys.h>
#include <kernel.h>

/// Checks whether a result code indicates success.
#define R_SUCCEEDED(res)   ((res)>=0)
/// Checks whether a result code indicates failure.
#define R_FAILED(res)      ((res)<0)
/// Returns the level of a result code.

#define FRAMEBUF_HEIGHT     544.0f
#define FRAMEBUF_WIDTH      960.0f

#define MAX_FILES 1024

#define FILES_PER_PAGE 10
#define ENTRY_SCALE .1f

#ifndef NDEBUG
#define printf sceClibPrintf
#else
#define printf 
#endif

typedef enum PlayerInitStatus {
    PLAYER_IDLE,
    PLAYER_LOADING,
    PLAYER_READY,
    PLAYER_ERROR
} PlayerInitStatus;

typedef enum PlayerState {
    PLAYER_PLAYING,
    PLAYER_PAUSED,
    PLAYER_STOPPED,
    PLAYER_FORWARD,
    PLAYER_BACKWARD,
    PLAYER_BUFFERING
} PlayerState;

typedef enum ThumbnailStatus {
    THUMBNAIL_NONE,
    THUMBNAIL_LOADING,
    THUMBNAIL_FINISHED
} ThumbnailStatus;

typedef enum SubtitlesStatus {
    SUBTITLES_DISABLED,
    SUBTITLES_ENABLED,
    SUBTITLES_NONE
} SubtitlesStatus;

extern vita2d_pvf *subtitleFont;
extern SceUInt32 SCE_CTRL_ENTER;
extern SceUInt32 SCE_CTRL_CANCEL;
extern SceUInt32 pressed;
extern vita2d_pgf *pgf;
extern void *mspaceGlobal;
extern int position;
extern int file_count;
extern char curDir[512];
extern char root_path[8];
extern PlayerInitStatus playerStatus;
extern PlayerState playerState;
extern ThumbnailStatus thumbnailStatus;
extern uint64_t streamDuration;
extern SubtitlesStatus subStatus;

#endif
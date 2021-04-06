#include "common.h"
#include "overlay.h"
#include "texture.h"

static int drawStatus()
{
    float scale = (FRAMEBUF_WIDTH*.05f)/200.0f;
    switch (playerState) {
        case PLAYER_PLAYING:
            vita2d_draw_texture_scale_rotate(play, FRAMEBUF_WIDTH/2, FRAMEBUF_HEIGHT*.9f, scale, scale, 0.0f);
            break;
        case PLAYER_PAUSED:
            vita2d_draw_texture_scale_rotate(pause, FRAMEBUF_WIDTH/2, FRAMEBUF_HEIGHT*.9f, scale, scale, 0.0f);
            break;
        case PLAYER_STOPPED:
            vita2d_draw_texture_scale_rotate(stop, FRAMEBUF_WIDTH/2, FRAMEBUF_HEIGHT*.9f, scale, scale, 0.0f);
            break;
        case PLAYER_BUFFERING:
            vita2d_draw_texture_scale_rotate(loading, FRAMEBUF_WIDTH/2, FRAMEBUF_HEIGHT*.9f, scale, scale, 0.0f);
            break;
        default:
            break;
    }
    switch (subStatus) {
        case SUBTITLES_DISABLED:
            vita2d_draw_texture_scale_rotate(subtitles_disabled, FRAMEBUF_WIDTH*.9f, FRAMEBUF_HEIGHT*.9f, scale, scale, 0.0f);
            break;
        case SUBTITLES_ENABLED:
            vita2d_draw_texture_scale_rotate(subtitles, FRAMEBUF_WIDTH*.9f, FRAMEBUF_HEIGHT*.9f, scale, scale, 0.0f);
            break;
        case SUBTITLES_NONE:
            vita2d_draw_texture_tint_scale_rotate(subtitles_disabled, FRAMEBUF_WIDTH*.9f, FRAMEBUF_HEIGHT*.9f, scale, scale, 0.0f, RGBA8(140,140,140,255));
            break;
    }
    return 0;
}

int drawOverlay(uint64_t currentTime)
{
    float percentageDone = currentTime/(float)streamDuration;
    vita2d_draw_rectangle(0,0,FRAMEBUF_WIDTH,FRAMEBUF_HEIGHT,RGBA8(0,0,0,120));
    vita2d_draw_rectangle(FRAMEBUF_WIDTH*.12f,FRAMEBUF_HEIGHT*.8f,FRAMEBUF_WIDTH*.76f, FRAMEBUF_HEIGHT*.027f, RGBA8(255,255,255,255));
    vita2d_draw_rectangle(FRAMEBUF_WIDTH*.12f,FRAMEBUF_HEIGHT*.8f,(FRAMEBUF_WIDTH*.76f)*percentageDone,FRAMEBUF_HEIGHT*.027f, RGBA8(120, 120, 255, 200));
    drawStatus();
    return 0;
}
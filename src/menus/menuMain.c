#include "main.h"
#include "common.h"
#include "utils.h"
#include "dir.h"
#include "avsound.h"

int initMainMenu()
{
    pgf = vita2d_load_default_pgf();
    subtitleFont = vita2d_load_custom_pvf("app0:OpenSans-Bold.ttf", (FRAMEBUF_HEIGHT*0.0290f), (FRAMEBUF_HEIGHT*0.0290f));
    getLastDirectory();
    getDirListing(SCE_FALSE);
    avSoundInit();
    return 0;
}

int drawMainMenu()
{
    vita2d_set_vblank_wait(1);
    while(1) {
        vita2d_start_drawing();
	    vita2d_clear_screen();
        vita2d_draw_rectangle(0, 0, FRAMEBUF_WIDTH, FRAMEBUF_HEIGHT, RGBA8(41, 41, 41, 255));
        displayFiles();
        vita2d_end_drawing();
        vita2d_wait_rendering_done();
		vita2d_end_shfb();
        readControls();
        handleDirControls();

        if (pressed == (SCE_CTRL_START | SCE_CTRL_SELECT))
			break;
    }
    return 0;
}
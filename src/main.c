#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "utils.h"
#include "texture.h"
#include "main.h"
#include "avplayer.h"

unsigned int sceLibcHeapSize = 90*1024*1024;

int initVita2d()
{
    vita2d_init_param params;
    memset(&params, 0, sizeof(vita2d_init_param));
    vita2d_init(&params);
    printf("Init\n");
    vita2d_set_clear_color(RGBA8(0, 0, 0, 255));
    initMainMenu();
    loadTextures();
    return 0;
}

int main()
{
    sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND);
    sceIoMkdir("ux0:data/SubPlayer", 0777);
    sceKernelLoadStartModule("app0:modules/libvita2d_sys.suprx", 0, SCE_NULL, 0, SCE_NULL, SCE_NULL);
    vita2d_display_set_max_resolution(FRAMEBUF_WIDTH, FRAMEBUF_HEIGHT);
    vita2d_display_set_resolution(FRAMEBUF_WIDTH, FRAMEBUF_HEIGHT);
    initVita2d();
    initAppUtil();
	SCE_CTRL_ENTER = getEnterButton();
	SCE_CTRL_CANCEL = getCancelButton();
    avPlayerInit();
    drawMainMenu();
    termAppUtil();
    vita2d_fini();
    freeTextures();

	sceKernelExitProcess(0);
	return 0;
}

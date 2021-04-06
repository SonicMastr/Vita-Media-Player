#include "common.h"
#include "utils.h"
#include <string.h>
#include <appmgr.h>
#include <kernel/processmgr.h>
#include <system_param.h>

static SceCtrlData pad, old_pad;

int readControls(void) {
	memset(&pad, 0, sizeof(SceCtrlData));
	sceCtrlPeekBufferPositive(0, &pad, 1);

	pressed = pad.buttons & ~old_pad.buttons;
	
	old_pad = pad;
	return 0;
}

void setMax(int *set, int value, int max) {
	if (*set > max)
		*set = value;
}

void setMin(int *set, int value, int min) {
	if (*set < min)
		*set = value;
}

int initAppUtil() {
	SceAppUtilInitParam init;
	SceAppUtilBootParam boot;
	memset(&init, 0, sizeof(SceAppUtilInitParam));
	memset(&boot, 0, sizeof(SceAppUtilBootParam));
	
	int ret = 0;
	
	if (R_FAILED(ret = sceAppUtilInit(&init, &boot)))
		return ret;

	if (R_FAILED(ret = sceAppUtilMusicMount()))
		return ret;
	
	return 0;
}

int termAppUtil() {
	int ret = 0;

	if (R_FAILED(ret = sceAppUtilMusicUmount()))
	
	if (R_FAILED(ret = sceAppUtilShutdown()))
		return ret;
	
	return 0;
}

int getEnterButton() {
	int button = 0;
	sceAppUtilSystemParamGetInt(2, &button);
	
	if (button == 0)
		return SCE_CTRL_CIRCLE;
	else
		return SCE_CTRL_CROSS;

	return 0;
}

int getCancelButton() {
	int button = 0;
	sceAppUtilSystemParamGetInt(2, &button);
	
	if (button == 0)
		return SCE_CTRL_CROSS;
	else
		return SCE_CTRL_CIRCLE;

	return 0;
}
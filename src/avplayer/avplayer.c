#include "avplayer.h"
#include "avsound.h"
#include "common.h"
#include "texture.h"
#include "overlay.h"
#include <string.h>
#include <display.h>
#include <kernel.h>
#include <libsysmodule.h>
#include "utils.h"
#include <videodec.h>

#define HOOK_NUM 5

typedef enum OverlayEvent {
	OVERLAY_ACTIVE = 1,
	OVERLAY_PAUSED = 2,
	OVERLAY_CLOSED = 4
} OverlayEvent;

int overlayEnable = 0;
SceAvPlayerHandle player;
vita2d_texture *frameTexture = NULL;
char *subs, *file;
int pictureInt;
int buffering_skip = 0;
float rad = 0.0f;
SceUID eventUid;
SceUID timerUid;
SceKernelSysClock timerBaseClock;
SceKernelSysClock *timerCurrentClock = NULL;
SceUID callback;

SceBool ran;

static void handleAVPlayerControls()
{
	if (pressed & ~(SCE_CTRL_TRIANGLE | SCE_CTRL_CROSS))
		sceKernelSetEventFlag(eventUid, OVERLAY_ACTIVE);
    int64_t	jumpToTimeOffset = (5000);
	if (pressed & SCE_CTRL_TRIANGLE) {
		if (overlayEnable) {
			sceKernelSetEventFlag(eventUid, OVERLAY_CLOSED);
		}
	}
	if (pressed & SCE_CTRL_SQUARE) {
		if (subStatus != 2) {
			if (subStatus)
				subStatus = SUBTITLES_DISABLED;
			else 
				subStatus = SUBTITLES_ENABLED;
		}
	}
		
    if (pressed & SCE_CTRL_CIRCLE)
        sceAvPlayerStop(player);
    if (pressed & SCE_CTRL_CROSS) {
        if (0>sceAvPlayerPause(player)) {
			sceAvPlayerResume(player);
			sceKernelSetEventFlag(eventUid, OVERLAY_ACTIVE);
		} else {
			sceKernelSetEventFlag(eventUid, OVERLAY_PAUSED);
		}
    }
    if (pressed & SCE_CTRL_LEFT) {
		int64_t jumpTime = sceAvPlayerCurrentTime(player);
		if(jumpTime <= jumpToTimeOffset)
			jumpTime = 0;
		else
		    jumpTime -= jumpToTimeOffset;
		sceAvPlayerJumpToTime(player, jumpTime);
	}
    if (pressed & SCE_CTRL_RIGHT) {
		int64_t jumpTime = sceAvPlayerCurrentTime(player);
		sceAvPlayerJumpToTime(player, jumpTime+jumpToTimeOffset);
	}
}

int avPlayerInit()
{
	int status = 0;
	int ret = sceKernelLoadStartModule("app0:/modules/reAvPlayer.suprx", sizeof(int), &buffering_skip, 0, NULL, &status);
	printf("## Start RE AV Player Module: %d\n", ret);
	subs = malloc(2048);
	memset(subs, '\0', 2048);

    return ret;
}

int avPlayerTerm()
{
	int ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_AVPLAYER);
	printf("## Stop AV Player Module: %d\n", ret);
    return ret;
}

void eventCallback(void* jumpback, int32_t argEventId, int32_t argSourceId, void* argEventData)
{
	switch (argEventId) {
	case SCE_AVPLAYER_STATE_READY :
	{
		int ret;
		const int streamCount = sceAvPlayerStreamCount(player);
		if (streamCount > 0) {
			printf("## %d Streams Found\n", streamCount);
			for (int i = 0; i < streamCount; i++) {
				SceAvPlayerStreamInfo StreamInfo;
				ret = sceAvPlayerGetStreamInfo(player, i, &StreamInfo);
				if (StreamInfo.type == SCE_AVPLAYER_VIDEO) {
					ret = sceAvPlayerEnableStream(player, i);
					printf("## Enable Video Stream: 0x%08x\n", ret);
					streamDuration = StreamInfo.duration;
				}
				else if (StreamInfo.type == SCE_AVPLAYER_AUDIO) {
					ret = sceAvPlayerEnableStream(player, i);
				}
				else if (StreamInfo.type == SCE_AVPLAYER_TIMEDTEXT) {
					ret = sceAvPlayerEnableStream(player, i);
					printf("## New subs. Code: %s\n", StreamInfo.details.subs.languageCode);
					subStatus = SUBTITLES_ENABLED;
				}
			}
		}
		buffering_skip = 0;
		sceAvPlayerStart(player);
		playerStatus = PLAYER_READY;
	}
	break;
	case SCE_AVPLAYER_TIMED_TEXT_DELIVERY:
 		{
			memset(subs, '\0', 2048);
 			SceAvPlayerFrameInfo* pTextEventData = (SceAvPlayerFrameInfo*)argEventData;
			uint16_t size = pTextEventData->details.subs.textSize;
			if ((char*)pTextEventData->pData) {
				strncpy(subs, (char*)pTextEventData->pData, size);
			}
 			printf("##Text to display: %s\n", subs);
 		}
    break;
    case SCE_AVPLAYER_STATE_PLAY:
		printf("## advanced_mp4_player: State Changed to SCE_AVPLAYER_STATE_PLAY\n");
		playerState = PLAYER_PLAYING;
		break;
	case SCE_AVPLAYER_STATE_STOP:
		printf("## advanced_mp4_player: State Changed to SCE_AVPLAYER_STATE_STOP\n");
		playerState = PLAYER_STOPPED;
		break;
	case SCE_AVPLAYER_STATE_PAUSE:
		printf("## advanced_mp4_player: State Changed to SCE_AVPLAYER_STATE_PAUSE\n");
		playerState = PLAYER_PAUSED;
		break;
	case SCE_AVPLAYER_STATE_BUFFERING:
		buffering_skip = 0;
		printf("## advanced_mp4_player: State Changed to SCE_AVPLAYER_STATE_BUFFERING\n");
		//playerState = PLAYER_BUFFERING;
		break;
	default:
		break;
	};
}

static SceInt32 audioOutThread(SceSize args, void *ThisObject)
{
	printf("## Audio Thread Initialized ##\n");
	uint8_t *noSound = (uint8_t *)memalign(0x20, 4096*4);
	memset(noSound, 0, 4096*4);
	SceAvPlayerFrameInfo audioFrame;
	memset(&audioFrame, 0, sizeof(SceAvPlayerFrameInfo));

	while(sceAvPlayerIsActive(player)) {
		if (sceAvPlayerGetAudioData(player, &audioFrame)) {
			avSoundOutput((uint8_t*)audioFrame.pData, audioFrame.details.audio.sampleRate, audioFrame.details.audio.channelCount);
		} else {
			avSoundOutput(noSound, 0, 0);
		}
	}

	avSoundOutputEnd();

	free(noSound);
	printf("## Audio Thread Killed ##\n");
	return sceKernelExitThread(0);
}

static SceInt32 loadAVPlayerThread()
{
    SceAvPlayerInitData playerInit;
    memset(&playerInit, 0, sizeof(SceAvPlayerInitData));
    playerInit.memoryReplacement.objectPointer = NULL;
	playerInit.memoryReplacement.allocate = Allocate;
	playerInit.memoryReplacement.deallocate = Deallocate;
	playerInit.memoryReplacement.allocateTexture= AllocateTexture;
	playerInit.memoryReplacement.deallocateTexture = DeallocateTexture;

    playerInit.basePriority = 125;
    playerInit.numOutputVideoFrameBuffers = 6;
    playerInit.eventReplacement.eventCallback = eventCallback;
	playerInit.autoStart = SCE_FALSE;
	playerInit.debugLevel = 3;

	player = sceAvPlayerInit(&playerInit);
	if (R_FAILED(sceAvPlayerAddSource(player, file))) {
		playerStatus = PLAYER_ERROR;
	}
	subStatus = SUBTITLES_NONE;
	return sceKernelExitThread(0);
}

static SceInt32 timerThread()
{
	while(sceAvPlayerIsActive(player))
	{
		if (!sceKernelPollEventFlag(eventUid, OVERLAY_ACTIVE, SCE_KERNEL_EVF_WAITMODE_CLEAR_ALL, NULL)) {
			sceKernelStopTimer(timerUid);
			sceKernelSetTimerTimeWide(timerUid, 0);
			sceKernelStartTimer(timerUid);
			printf("## Timer Thread running. Time: %d\n", sceKernelGetTimerTimeWide(timerUid));
			overlayEnable = 1;
		}
		else if (!sceKernelPollEventFlag(eventUid, OVERLAY_PAUSED, SCE_KERNEL_EVF_WAITMODE_CLEAR_ALL, NULL)) {
			sceKernelStopTimer(timerUid);
			sceKernelSetTimerTimeWide(timerUid, 0);
			overlayEnable = 1;
		}
		else if (!sceKernelPollEventFlag(eventUid, OVERLAY_CLOSED, SCE_KERNEL_EVF_WAITMODE_CLEAR_ALL, NULL)) {
			sceKernelStopTimer(timerUid);
			sceKernelSetTimerTimeWide(timerUid, 0);
			overlayEnable = 0;
		}
		if (overlayEnable && sceKernelGetTimerTimeWide(timerUid) > 4*1000*1000)
			sceKernelSetEventFlag(eventUid, OVERLAY_CLOSED);


		sceKernelDelayThread(10 * 1000);
	}
	printf("## Timer Thread Killed ##\n");
	return sceKernelExitThread(0);
}

static int drawSubtitles()
{
	if (subs[0] != '\0') {
		int fontSize = 1;
		float linespace = FRAMEBUF_HEIGHT/200.0f;
		int width = vita2d_pvf_text_width(subtitleFont, fontSize, subs);
		int height = vita2d_pvf_text_height(subtitleFont, fontSize, subs);
		int x = FRAMEBUF_WIDTH/2;
		int y = FRAMEBUF_HEIGHT - ((fontSize*2) + ((fontSize*2)/8));
		if (height > fontSize*2)
			y=y-(height-(fontSize*2));
		vita2d_pvf_draw_text(subtitleFont, x+2 - (width / 2), y+2, RGBA8(0,0,0,255), fontSize, subs);
		vita2d_pvf_draw_text(subtitleFont, x+2 - (width / 2), y-2, RGBA8(0,0,0,255), fontSize, subs);
		vita2d_pvf_draw_text(subtitleFont, x-2 - (width / 2), y+2, RGBA8(0,0,0,255), fontSize, subs);
		vita2d_pvf_draw_text(subtitleFont, x-2 - (width / 2), y-2, RGBA8(0,0,0,255), fontSize, subs);
		vita2d_pvf_draw_text(subtitleFont, x - (width / 2), y, RGBA8(255,255,255,255), fontSize, subs);
	}
	return 0;
}

static int drawFrame(SceUInt32 width, SceUInt32 height)
{
    float scale, startX;
    scale = FRAMEBUF_HEIGHT/(float)height;
    startX = -((((float)width*scale)-FRAMEBUF_WIDTH)/2);
	vita2d_texture_set_filters(frameTexture, SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR);
    vita2d_draw_texture_scale(frameTexture, startX, 0, scale, scale);
	if (subStatus == SUBTITLES_ENABLED)
		drawSubtitles();
    return 0;
}

static int drawLoading()
{
	float scale = (FRAMEBUF_WIDTH*.15f)/200.0f;
	vita2d_start_drawing();
	vita2d_clear_screen();
	vita2d_draw_texture_scale_rotate(loading, (FRAMEBUF_WIDTH/2.0f), (FRAMEBUF_HEIGHT/2.0f), scale, scale, rad);
	vita2d_end_drawing();
	vita2d_end_shfb();
	return 0;
}

int startPlayback(char *filename)
{
	SceUID main_thread_uid = sceKernelGetThreadId();
    sceKernelChangeThreadPriority(main_thread_uid, 70);
	file = filename;
	rad = 0.0f;
	playerStatus = PLAYER_LOADING;
	SceUID AVPlayerThreadId = 0;
    SceUID audioThreadId = 0;
	SceUID timerThreadId = 0;
	eventUid = sceKernelCreateEventFlag("OverlayEvent", SCE_KERNEL_ATTR_MULTI, 0, NULL);
	timerUid = sceKernelCreateTimer("OverlayTimer", 0, NULL);
	AVPlayerThreadId = sceKernelCreateThread("loadAVPlpayer", loadAVPlayerThread, 0x10000100, 0x4000, 0, 0, NULL);
	sceKernelStartThread(AVPlayerThreadId, 0, NULL);
	while (playerStatus == PLAYER_LOADING) {
		sceDisplayWaitVblankStart();
		drawLoading();
		rad += 0.1f;
	}
    if (playerStatus == PLAYER_ERROR) {
        printf("Whoops\n");
    }
    else if (playerStatus == PLAYER_READY){
		frameTexture = malloc(sizeof(*frameTexture));
    	sceAvPlayerSetLooping(player, SCE_FALSE);
		audioThreadId = sceKernelCreateThread("AudioOutput", audioOutThread, 0x10000100, 0x4000, 0, 0, NULL);
    	sceKernelStartThread(audioThreadId, 0, NULL);
		timerThreadId = sceKernelCreateThread("OverlayThread", timerThread, 0x10000100, 0x4000, 0, 0, NULL);
		sceKernelStartThread(timerThreadId, 0, NULL);
		sceKernelSetEventFlag(eventUid, OVERLAY_ACTIVE);
        SceAvPlayerFrameInfo videoFrame;
        memset(&videoFrame, 0, sizeof(SceAvPlayerFrameInfo));
        while (sceAvPlayerIsActive(player)) {
			sceDisplayWaitVblankStart();
			vita2d_start_drawing();
			vita2d_clear_screen();
            if (sceAvPlayerGetVideoData(player, &videoFrame)) {
				sceGxmTextureInitLinear(&frameTexture->gxm_tex, videoFrame.pData, SCE_GXM_TEXTURE_FORMAT_YVU420P2_CSC1, videoFrame.details.video.width, videoFrame.details.video.height, 0);
			}
			if (frameTexture != NULL)
				drawFrame(videoFrame.details.video.width, videoFrame.details.video.height);
			if (overlayEnable)
				drawOverlay(sceAvPlayerCurrentTime(player));
			vita2d_end_drawing();
			vita2d_wait_rendering_done();
			vita2d_end_shfb();
            readControls();
            handleAVPlayerControls();
        }
    }
    sceAvPlayerClose(player);
	printf("Freeing?\n\n");
	if (frameTexture != NULL)
        free(frameTexture);
	sceKernelDeleteEventFlag(eventUid);
	sceKernelDeleteTimer(timerUid);
    return 0;
}

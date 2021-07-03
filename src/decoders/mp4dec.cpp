#include <decoder.hpp>
#include <decoders/mp4dec.hpp>
#include <utils.hpp>

using namespace vmp::decoder;
using namespace vmp::utils;
using namespace vmp::audio;


void loadCallback(void* jumpback, SceInt32 argEventId, SceInt32 argSourceId, void* argEventData)
{
    Mp4Dec *decoder = static_cast<Mp4Dec *>(jumpback);
	switch (argEventId) {
	case SCE_AVPLAYER_STATE_READY :
        {
            int ret;
            const int streamCount = sceAvPlayerStreamCount(decoder->getPlayer());
            if (streamCount > 0) {
                printf("## %d Streams Found\n", streamCount);
                for (int i = 0; i < streamCount; i++) {
                    SceAvPlayerStreamInfo StreamInfo;
                    ret = sceAvPlayerGetStreamInfo(decoder->getPlayer(), i, &StreamInfo);
                    if (StreamInfo.type == SCE_AVPLAYER_VIDEO) {
                        ret = sceAvPlayerEnableStream(decoder->getPlayer(), i);
                        printf("## Enable Video Stream: 0x%08x\n", ret);
                        decoder->setStreamDuration(StreamInfo.duration);
                    }
                    else if (StreamInfo.type == SCE_AVPLAYER_AUDIO) {
                        ret = sceAvPlayerEnableStream(decoder->getPlayer(), i);
                        Audio::getInstance()->createThread();
                    }
                    else if (StreamInfo.type == SCE_AVPLAYER_TIMEDTEXT) {
                        ret = sceAvPlayerEnableStream(decoder->getPlayer(), i);
                        printf("## New subs. Code: %s\nReturn: %d\n", StreamInfo.details.subs.languageCode, ret);
                        decoder->setSubs(true);
                    }
                }
            }
           decoder->setStatus(DECODER_READY);
           decoder->setCurrentTime();
        }
	    break;
	case SCE_AVPLAYER_TIMED_TEXT_DELIVERY:
 		{
            printf("Doing this\n");
			decoder->setSubtitleData(NULL, 0);
            printf("Nice\n");
 			SceAvPlayerFrameInfo* pTextEventData = (SceAvPlayerFrameInfo*)argEventData;
			SceUShort16 size = pTextEventData->details.subs.textSize;
			if ((char*)pTextEventData->pData) {
                printf("strncpy\n");
				decoder->setSubtitleData((char*)pTextEventData->pData, size);
                printf("done\n");
			}
 			printf("##Text to display: %s\n", decoder->getSubtitleData());
 		}
        break;
    case SCE_AVPLAYER_STATE_PLAY:
		printf("## advanced_mp4_player: State Changed to SCE_AVPLAYER_STATE_PLAY\n");
		decoder->setState(DECODER_PLAYING);
        decoder->setCurrentTime();
		break;
	case SCE_AVPLAYER_STATE_STOP:
		printf("## advanced_mp4_player: State Changed to SCE_AVPLAYER_STATE_STOP\n");
		decoder->setState(DECODER_STOPPED);
        decoder->setCurrentTime();
		break;
	case SCE_AVPLAYER_STATE_PAUSE:
		printf("## advanced_mp4_player: State Changed to SCE_AVPLAYER_STATE_PAUSE\n");
		decoder->setState(DECODER_PAUSED);
        decoder->setCurrentTime();
		break;
	case SCE_AVPLAYER_STATE_BUFFERING:
		printf("## advanced_mp4_player: State Changed to SCE_AVPLAYER_STATE_BUFFERING\n");
		//playerState = PLAYER_BUFFERING;
		break;
	default:
		break;
	};
}

void Mp4Dec::setCurrentTime(void) { _currentTime = sceAvPlayerCurrentTime(_player);}

void Mp4Dec::setStatus(DecoderStatus status) { _status = status; };

void Mp4Dec::setState(DecoderState state) { _state = state; };

void Mp4Dec::setStreamDuration(SceUInt64 time) { _streamDuration = time; };

void Mp4Dec::setSubtitleData(char *data, SceUShort16 size)
{
    if (data != NULL)
        strncpy(_subs, data, size);
    else {
        memset(_subs, '\0', 2048);
    }
}

void Mp4Dec::setSubs(SceBool subs)
{
    _hasSubs = subs;
}

SceBool Mp4Dec::getVideoData(DecoderVideoData *data)
{
    if (is_active())
    {
        setCurrentTime();
        SceBool isData = sceAvPlayerGetVideoData(_player, &videoFrame);
        if (isData)
        {
            data->frameData = (SceUInt8 *)videoFrame.pData;
            data->width = videoFrame.details.video.width;
            data->height = videoFrame.details.video.height;
            data->aspectRatio = videoFrame.details.video.aspectRatio;
            data->timeStamp = videoFrame.timeStamp;
        }
        return isData;
    }
}

SceBool Mp4Dec::getAudioData(DecoderAudioData *data)
{
    if (is_active())
    {
        SceBool isData = sceAvPlayerGetAudioData(_player, &audioFrame);
        if (isData)
        {
            data->frameData = (SceUInt8 *)audioFrame.pData;
            data->channelCount = audioFrame.details.audio.channelCount;
            data->sampleRate = audioFrame.details.audio.sampleRate;
            data->size = audioFrame.details.audio.size;
            data->timeStamp = audioFrame.timeStamp;
        }
        return isData;
    }
}

SceBool Mp4Dec::is_active(void)
{
    return sceAvPlayerIsActive(_player);
}

SceInt32 Mp4Dec::start(void)
{
    return sceAvPlayerStart(_player);
}

void Mp4Dec::initialize(void)
{
    sceKernelLoadStartModule("app0:/modules/reAvPlayer.suprx", sizeof(int), NULL, 0, NULL, NULL);
    SceAvPlayerInitData playerInit;
    memset(&playerInit, 0, sizeof(SceAvPlayerInitData));
    playerInit.memoryReplacement.objectPointer = NULL;
	playerInit.memoryReplacement.allocate = mem::Allocate;
	playerInit.memoryReplacement.deallocate = mem::Deallocate;
	playerInit.memoryReplacement.allocateTexture= mem::AllocateTexture;
	playerInit.memoryReplacement.deallocateTexture = mem::DeallocateTexture;

    playerInit.basePriority = 125;
    playerInit.numOutputVideoFrameBuffers = 6;
    playerInit.eventReplacement.objectPointer = this;
    playerInit.eventReplacement.eventCallback = loadCallback;
	playerInit.autoStart = SCE_FALSE;
	playerInit.debugLevel = SceAvPlayerDebuglevels::SCE_AVPLAYER_DBG_ALL;

	memset(_subs, '\0', 2048);

    _player = sceAvPlayerInit(&playerInit);
    if (0 > sceAvPlayerAddSource(_player, _fileName.c_str()))
        _status = DECODER_ERROR;

}

// Create and Initialize the AVPlayer Decoder
Mp4Dec::Mp4Dec(std::string fileName)
{
    _fileName = fileName;
    _status = DECODER_LOADING;
    initialize();
    memset(&videoFrame, 0, sizeof(SceAvPlayerFrameInfoEx));
}

Mp4Dec::~Mp4Dec(void)
{
}

#include <audio.hpp>

using namespace vmp::audio;
using namespace vmp::player;
using namespace vmp::decoder;

SceUInt8 *silent = NULL;

SceInt32 audioThread(SceSize args, void* argp)
{
    Audio *audio = Audio::getInstance();
    Decoder *decoder = Player::getInstance()->getDecoder();

    while (decoder->is_active())
    {
        DecoderAudioData data;
        if (decoder->getAudioData(&data))
            audio->queue(&data);
        else
            audio->queue(NULL);
    }

    return sceKernelExitThread(0);
}

Audio *Audio::getInstance()
{
    static Audio *_instance;  // This is dumb. Maybe just move this
    if (!_instance)
        _instance = new Audio;
    return _instance;
}

void Audio::createThread()
{
    _threadID = sceKernelCreateThread("Audio Thread", audioThread, 0x10000100, 0x4000, 0, 0, SCE_NULL);
    sceKernelStartThread(_threadID, 0, SCE_NULL);
}


void Audio::queue(DecoderAudioData *audioData)
{
    static int channelType;
    if (audioData != NULL) {
        if ((audioData->sampleRate != _sampleRate) || (_channels != audioData->channelCount) || (SINGLE_PCM_BUFFER*audioData->channelCount != _pcmBufferSize)) {
                if (audioData->channelCount == 1)
                    channelType = SCE_AUDIO_OUT_PARAM_FORMAT_S16_MONO;
                if (audioData->channelCount == 2)
                    channelType = SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO;

                _pcmBufferSize = SINGLE_PCM_BUFFER*audioData->channelCount;
                _channels = audioData->channelCount;
                _sampleRate = audioData->sampleRate;

                sceAudioOutSetConfig(_portId,(_pcmBufferSize/_channels/sizeof(SceShort16)), _sampleRate, channelType);
	    }
        sceAudioOutOutput(_portId, audioData->frameData);
    }
    else {
        sceAudioOutOutput(_portId, silent);
    }
}

Audio::Audio()
{
    silent = (SceUInt8 *)memalign(0x20, 4096*4);
    memset(silent, 0, 4096*4);
	_portId = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, (_pcmBufferSize/_channels/sizeof(SceShort16)), _sampleRate, SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);
}

Audio::~Audio()
{
    if (_portId > -1)
        sceAudioOutReleasePort(_portId);
}

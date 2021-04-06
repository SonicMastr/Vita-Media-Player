#include "avsound.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <audioout.h>

#define SINGLE_PCM_BUFFER 2048

// Be default we assume 48kHz 2channel output but the below code will change to the correct settings if required.
static int sampleRate = 48000;
static int sampleChannels = 2;
static int pcmBufferSize = SINGLE_PCM_BUFFER*2;
static int portId = -1;

int avSoundInit()
{
	const int portType = SCE_AUDIO_OUT_PORT_TYPE_BGM;
	portId = sceAudioOutOpenPort(portType, (pcmBufferSize/sampleChannels/sizeof(int16_t)), sampleRate, SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);
	if((int)portId < 0){
		printf("## Failed to Open Audio Port: %d ##\n", portId);
		return -1;
	}
	return 0;
}

int avSoundShutdown()
{
	int ret = sceAudioOutReleasePort(portId);
	printf("## Release Audio Port: %d ##\n", ret);
	return ret;
}

int avSoundOutput(uint8_t* argpData, uint32_t argSampleRate, uint32_t argChannelCount)
{
	static int channelType;

	if (portId < 0)
		return 0;

	if ((((int)argSampleRate != sampleRate) || (sampleChannels != argChannelCount) || (SINGLE_PCM_BUFFER*argChannelCount != pcmBufferSize))
		&&(argChannelCount != 0) && (argSampleRate != 0)){
		if (argChannelCount == 1)
			channelType = SCE_AUDIO_OUT_PARAM_FORMAT_S16_MONO;
		if (argChannelCount == 2)
			channelType = SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO;

		pcmBufferSize = SINGLE_PCM_BUFFER*argChannelCount;
		sampleChannels = argChannelCount;
		sampleRate = argSampleRate;

		sceAudioOutSetConfig(portId,(pcmBufferSize/sampleChannels/sizeof(int16_t)), sampleRate, channelType);
		printf("SAMPLE RATE CHANGED TO %d\n",sampleRate);
	}

	if (0 > sceAudioOutOutput(portId, argpData))
		return -1;

	return 0;
}

int avSoundOutputEnd()
{
	if (0>sceAudioOutOutput(portId, NULL))
		return -1;

	return 0;
}

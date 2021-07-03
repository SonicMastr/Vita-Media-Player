#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <common.hpp>
#include <player.hpp>
#include <decoder.hpp>
#include <audioout.h>

#define SINGLE_PCM_BUFFER 2048

namespace vmp { namespace audio {
    class Audio {
        private:
            SceUInt32 _sampleRate = 48000;
            SceUInt32 _channels = 2;
            SceUInt32 _pcmBufferSize = SINGLE_PCM_BUFFER*2;
            SceInt32 _portId = -1;
            SceUID _threadID = -1;

            Audio(void);
            ~Audio(void);
        public:
            static Audio *getInstance(void);
            void setChannels(SceUInt32 channels);
            void setSampleRate(SceUInt32 sampleRate);
            void queue(vmp::decoder::DecoderAudioData *audioData);
            void createThread(void);
    };
}};

#endif /* AUDIO_HPP */
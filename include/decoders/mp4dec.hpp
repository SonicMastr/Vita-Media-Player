#ifndef MP4DEC_HPP
#define MP4DEC_HPP

#include <common.hpp>
#include <audio.hpp>
#include <utils.hpp>
#include <stdint.h>
#include <sceavplayer.h>
#include <sceavplayer_ex.h>

namespace vmp { namespace decoder {

    class Mp4Dec : public Decoder
    {
            void initialize(void);
        private:
            SceAvPlayerHandle _player;
            SceAvPlayerFrameInfo videoFrame;
            SceAvPlayerFrameInfo audioFrame;

        public:
            void setCurrentTime(void);
            SceAvPlayerHandle getPlayer(void) const { return _player; };
            void setStatus(DecoderStatus status);
            void setState(DecoderState state);
            void setStreamDuration(SceUInt64 time);
            void setSubtitleData(char *data, SceUShort16 size);
            void setSubs(SceBool subs);

            SceBool getVideoData(DecoderVideoData *data);
            SceBool getAudioData(DecoderAudioData *data);
            char *getSubtitleData(void) { return _subs; };

            SceBool is_active(void);
            SceBool has_subtitles(void) { return _hasSubs; };

            SceInt32 start(void);
            Mp4Dec(std::string fileName);
            ~Mp4Dec(void);
    };
}};

#endif /* MP4DEC_HPP */

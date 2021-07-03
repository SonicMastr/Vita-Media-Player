#ifndef DECODER_HPP
#define DECODER_HPP

#include <common.hpp>

namespace vmp { namespace decoder {

    enum DecoderType {
        DECODER_NONE = 0,
        DECODER_MP4,
    };

    enum DecoderStatus {
        DECODER_IDLE,
        DECODER_LOADING,
        DECODER_READY,
        DECODER_ERROR
    };

    enum DecoderState {
        DECODER_PLAYING,
        DECODER_PAUSED,
        DECODER_STOPPED,
        DECODER_FORWARD,
        DECODER_BACKWARD,
        DECODER_BUFFERING
    };

    struct DecoderVideoData {
        SceUInt8*   frameData;
        SceUInt64   timeStamp;
        SceUInt32   width;
		SceUInt32	height;
		float		aspectRatio; // optional
		SceUInt8	languageCode[4]; // optional
    };

    struct DecoderAudioData {
        SceUInt8*   frameData;
        SceUInt64   timeStamp;
        SceUInt16	channelCount;
		SceUInt32	sampleRate;
		SceUInt32	size;
		SceUInt8	languageCode[4]; // optional
    };

    class Decoder {
        protected:
            std::string _fileName;
            SceUInt64 _currentTime, _streamDuration;
            SceBool _isActive, _hasSubs;
            DecoderStatus _status;
            DecoderState _state;
            char _subs[2048];
            virtual void initialize(void) = 0;

        public:
            /** Getters */
            virtual SceUInt64 getCurrentTime(void) const { return _currentTime; };
            virtual SceUInt64 getStreamDuration(void) const { return _streamDuration; };
            virtual SceBool getVideoData(DecoderVideoData *data) {};
            virtual SceBool getAudioData(DecoderAudioData *data) {};
            virtual char *getSubtitleData(void) {};
            virtual DecoderStatus getStatus(void) const { return _status; };
            virtual DecoderState getState(void) const { return _state; };

            /** Informational Variables **/
            virtual SceBool is_active(void) { return _isActive; };
            virtual SceBool has_subs(void) { return _hasSubs; };

            /** Methods **/
            virtual SceInt32 start(void) {};
            virtual SceInt32 pause(void) {};
            virtual SceInt32 resume(void) {};
            virtual SceInt32 stop(void) {};
            virtual SceInt32 jumpTo(SceUInt64 timeMsec) {};
            Decoder(void) {};
            virtual ~Decoder(void) {};
    };
}};

#include <decoders/mp4dec.hpp>

#endif /* DECODER_HPP */

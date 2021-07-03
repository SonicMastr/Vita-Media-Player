#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <common.hpp>
#include <decoder.hpp>

namespace vmp { namespace player {

    class Player {
        private:
            std::string _fileName;
            vmp::decoder::DecoderType _type;
            vmp::decoder::Decoder *_decoder = NULL;
            Player(void);
            ~Player(void);
        public:
            static Player *getInstance(void);
            vmp::decoder::Decoder* getDecoder(SceBool reset = 0);
            void createDecoder(std::string fileName);
    };

}};

#endif /* PLAYER_HPP */

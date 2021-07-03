#include <player.hpp>
#include <utils.hpp>

using namespace vmp::player;
using namespace vmp::decoder;
using namespace vmp::utils;

Player *Player::getInstance()
{
    static Player *_instance; // This is dumb. Maybe just move this
    if (!_instance)
        _instance = new Player;
    return _instance;
}

vmp::decoder::Decoder* Player::getDecoder(SceBool reset)
{
    if (_decoder != NULL && reset)
    {  
        delete _decoder;
        _decoder = NULL;
    }
    if (_decoder == NULL)
    {
        switch (_type)
        {
            case DecoderType::DECODER_MP4:
                _decoder = new Mp4Dec(_fileName);
                break;
            default:
                return NULL;
                break;
        }
    }
    return _decoder;
};

void Player::createDecoder(std::string fileName)
{
    _fileName = fileName;
    transform(fileName.begin(), fileName.end(), fileName.begin(), std::toupper);
    printf("Creating player with filename: %s\n", fileName.c_str());
    if (!file::getExtension(fileName).compare(".MP4"))
        _type = DecoderType::DECODER_MP4;
    else
        _type = DecoderType::DECODER_NONE;
};

Player::Player(void)
{
};

Player::~Player(void)
{
};
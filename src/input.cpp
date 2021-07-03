#include <player.hpp>
#include <input.hpp>

using namespace vmp::input;

Input *Input::getInstance(void)
{
    static Input *_instance; // This is dumb. Maybe just move this
    if (!_instance)
        _instance = new Input;
    return _instance;
}

void Input::poll(void)
{
    sceCtrlPeekBufferPositive(0, &_new, 1);
    _analog.lx = _new.lx;
    _analog.ly = _new.ly;
    _analog.rx = _new.rx;
    _analog.ry = _new.ry;
};

AnalogData Input::getAnalog(void)
{
    return _analog;
};

SceBool Input::pressed(SceUInt32 buttons)
{
    if (((buttons & _new.buttons) == buttons) && ((buttons & _old.buttons) != buttons))
        return SCE_TRUE;
    return SCE_FALSE;
};

SceBool Input::released(SceUInt32 buttons)
{
    if (((buttons & _old.buttons) == buttons) && ((buttons & _new.buttons) != buttons))
        return SCE_TRUE;
    return SCE_FALSE;
}

SceBool Input::held(SceUInt32 buttons)
{
    if ((buttons & _new.buttons) == buttons)
        return SCE_TRUE;
    return SCE_FALSE;
}


Input::Input()
{
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG_WIDE);
}

Input::~Input()
{
}
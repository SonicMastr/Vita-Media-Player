#include <menu.hpp>
#include <menus/browser.hpp>
#include <utils.hpp>

using namespace vmp::menu;
using namespace vmp::utils;
using namespace vmp::gfx;
using namespace vmp::input;

static vita2d_pvf *font;

void Browser::update(void)
{
    _input->poll();
    _gfx->beginFrame();
    if (_input->held(SCE_CTRL_CROSS))
        _gfx->drawText(font, 200, 200, RGBA8(255,0,0,255), "You are no longer gay");
    else
        _gfx->drawText(font, 200, 200, RGBA8(0,255,0,255), "Ha. Gay");
    _gfx->endFrame();
}

Browser::Browser(void)
{
    _gfx = Graphics::getInstance();
    _input = Input::getInstance();

    font = _gfx->loadFontFromFile("app0:OpenSans-Bold.ttf", (_gfx->getScreenHeight()*0.0290f), (_gfx->getScreenHeight()*0.0290f));
}

Browser::~Browser(void)
{
}


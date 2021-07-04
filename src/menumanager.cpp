#include <menu.hpp>
#include <gfx.hpp>

using namespace vmp::menu;
using namespace vmp::gfx;

Menu *MenuManager::loadMenu(void)
{
    Menu *temp;
    switch (_currentMenuType)
    {
        case MENU_BROWSER:
            temp = new Browser();
            break;
        case MENU_PLAYER:
        case MENU_SETTINGS:
        case MENU_NONE:
        default:
            break;
    }
    _currentMenuType = MENU_NONE;
    return temp;
}

void MenuManager::setMenu(MenuType type)
{
    _currentMenuType = type;
}

MenuManager *MenuManager::getInstance(void)
{
    static MenuManager *_instance; // This is dumb. Maybe just move this
    if (!_instance)
        _instance = new MenuManager;
    return _instance;
}

MenuManager::MenuManager(void)
{
    _currentMenuType = MENU_BROWSER;

    while (_currentMenuType != MENU_DONE)
    {
        if (_currentMenuType != MENU_NONE)
        {
            _currentMenu = loadMenu();
        }
        _currentMenu->update();
    }
    sceKernelExitProcess(0);
}

MenuManager::~MenuManager(void)
{
}

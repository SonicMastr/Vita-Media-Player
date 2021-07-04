#ifndef MENU_HPP
#define MENU_HPP

#include <common.hpp>
#include <kernel.h>
#include <gfx.hpp>
#include <input.hpp>

namespace vmp { namespace menu {

    enum MenuType {
            MENU_NONE = 0,
            MENU_BROWSER,
            MENU_PLAYER,
            MENU_SETTINGS,
            MENU_DONE
    };

    class Menu {
        protected:
            vmp::gfx::Graphics *_gfx;
            vmp::input::Input *_input;
        public:
            virtual void update(void) {};
            Menu(void) {};
            ~Menu(void) {};
    };

    class MenuManager {

        private:
            MenuType _currentMenuType;
            Menu *_currentMenu;
            MenuManager(void);
            ~MenuManager(void);

            Menu *loadMenu(void);

        public:
            void setMenu(MenuType type);
            static MenuManager *getInstance(void);
    };

}};

#include <menus/browser.hpp>

#endif /* MENU_HPP */
#ifndef GFX_HPP
#define GFX_HPP

#include <common.hpp>
#include <kernel.h>
#include <gfx.hpp>



namespace vmp { namespace menu {

    enum MenuType {
            MENU_NONE = 0,
            MENU_MAIN,
            MENU_SETTINGS
        }

    class Menu {
        protected:
            Graphics *_graphics;
        public:
            void draw(void);
            Menu(Graphics* graphicsObj);
            ~Menu(void);
    }

    class MenuFactory {

        private:
            Graphics *_graphics;
        public:
            Menu getMenu(MenuType type);
            MenuFactory(Graphics* graphicsObj);
            ~MenuFactory(void);
    };

}};

#endif /* GFX_HPP */
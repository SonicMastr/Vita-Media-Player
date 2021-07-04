#ifndef BROWSER_HPP_HPP
#define BROWSER_HPP_HPP

#include <common.hpp>
#include <kernel.h>

namespace vmp { namespace menu {

    class Browser : public Menu {
        public:
            void update(void);
            Browser(void);
            ~Browser(void);
    };

}};

#endif /* BROWSER_HPP */

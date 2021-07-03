#ifndef UTILS_HPP
#define UTILS_HPP

#include <common.hpp>
#include <kernel.h>

namespace vmp { namespace utils {

    namespace mem {

        void* AllocateTexture(void* jumpback, uint32_t alignment, uint32_t size);
        void DeallocateTexture(void* jumpback, void* pMemory);
        void* Allocate(void* jumpback, uint32_t alignment, uint32_t size);
        void Deallocate(void* jumpback, void* pMemory);

    }

    namespace file {

        SceBool vid_is_valid(std::string filename);
        SceBool sub_is_valid(std::string filename);
        std::string *sub_convert(std::string *filename, int type);
        std::string getExtension(std::string filename);
        SceUInt32 getExtensionLength(std::string filename);

    }

}};

#endif /* UTILS_HPP */

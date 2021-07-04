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

        class FileEntry {

            private:
                std::string _name, _path;
                SceDateTime _creationTime;
                SceBool _directory;
            public:
                std::string getFileName(void) const { return _name; };
                std::string getFilePath(void) const { return _path; };
                SceDateTime getFileCreationTime(void) const { return _creationTime; };
                SceBool is_directory(void) const { return _directory; };
                FileEntry(std::string filename, std::string filepath, SceDateTime creationTime, SceBool directory);
                ~FileEntry(void);

        };

        class FileSystem {

            private:
                std::string _currentDirectory;
            public:
                std::string getParentDirectory(void);
                std::string getCurrentDirectory(void);
                std::vector<FileEntry> getDirectoryContents(std::string directoryPath, SceBool unsupported = SCE_FALSE);
        };

    }

}};

#endif /* UTILS_HPP */

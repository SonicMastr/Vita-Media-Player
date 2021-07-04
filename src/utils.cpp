#include <utils.hpp>
#include <gxm.h>

#define ROUND_UP(x, a)	((((unsigned int)x)+((a)-1u))&(~((a)-1u)))

using namespace vmp::utils;

std::vector <std::string> vid_extensions {".mp4"};
std::vector <std::string> sub_extensions {".srt", ".ass"};

void *avTextureAlloc(SceKernelMemBlockType type, unsigned int size, unsigned int alignment, unsigned int attribs, SceUID *uid)
{
	int ret = 0;
	void *mem = NULL;
	SceKernelAllocMemBlockOpt opt;

	(void)ret;

	/*E page align the size */
	if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW) {
		/*E CDRAM memblocks must be 256KiB aligned */

		if (alignment < 0x40000)
			alignment = 0x40000;
		size = ROUND_UP(size, alignment);

	} else if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW) {
		/*E LPDDR PHYCONT memblocks must be 1MiB aligned */

		if (alignment < 0x100000)
			alignment = 0x100000;
		size = ROUND_UP(size, alignment);

	} else {
		/*E LPDDR memblocks must be 4KiB aligned */
		if (alignment < 0x1000)
			alignment = 0x1000;
		size = ROUND_UP(size, 0x1000);
	}

	if(type == SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW) {
		*uid = sceKernelAllocMemBlock("vmpg", type, size, NULL);
	} else {
		memset(&opt, 0, sizeof(opt));
		opt.size      = sizeof(SceKernelAllocMemBlockOpt);
		opt.attr      = 0x00000004U;
		opt.alignment = alignment;
		*uid = sceKernelAllocMemBlock("vmpg", type, size, &opt);
	}

	printf("Alloc UID %d\n", *uid);

	ret = sceKernelGetMemBlockBase(*uid, &mem);

	ret = sceGxmMapMemory(mem, size, attribs);

	return mem;
}

void avTextureFree(void *mem, SceUID memUid)
{
	int ret;
	SceUID uid;

	if (memUid == SCE_UID_INVALID_UID) {
		uid = sceKernelFindMemBlockByAddr(mem, 0);
	} else {
		uid = memUid;
		if (mem == NULL) {
			ret = sceKernelGetMemBlockBase(uid, &mem);
		}
	}

	ret = sceGxmUnmapMemory(mem);

	ret = sceKernelFreeMemBlock(uid);
}

void* mem::AllocateTexture(void* jumpback, uint32_t alignment, uint32_t size)
{
    SceUID textureMemoryUID;

	return avTextureAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA,
						size, alignment,
						SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
						&textureMemoryUID);
}

void mem::DeallocateTexture(void* jumpback, void* pMemory)
{
	avTextureFree(pMemory, SCE_UID_INVALID_UID);
};

void* mem::Allocate(void* jumpback, uint32_t alignment, uint32_t size)
{
	return memalign(alignment, size);
};

void mem::Deallocate(void* jumpback, void* pMemory)
{
	free(pMemory);
};

std::string file::getExtension(std::string filename)
{
	int index = filename.find_last_of('.');
	return filename.substr(index, filename.length() - index);
};

SceUInt32 file::getExtensionLength(std::string filename)
{
	return file::getExtension(filename).length();
};

SceBool file::vid_is_valid(std::string filename)
{
	std::string extension = file::getExtension(filename);
	printf("Welp\n");
	for (std::string& ext : vid_extensions)
	{
		if (!extension.compare(ext)) {
			printf("Yes?\n");
			return SCE_TRUE;
		}
	}
	printf("No?\n");
	return SCE_FALSE;
};

std::string *file::sub_convert(std::string *filename, int type)
{
	if (file::vid_is_valid(*filename))
	{
		printf("Kill me now %s\n", sub_extensions.at(type - 2).c_str());
		SceUID file = sceIoOpen(filename->replace(filename->find_last_of('.'), sub_extensions.at(type - 2).length(), sub_extensions.at(type - 2)).c_str(), SCE_O_RDONLY, 0777);
		if (file >= 0) {
			sceIoClose(file);
			return filename;
		}
	}
	return NULL;
};

SceBool file::sub_is_valid(std::string filename)
{
	transform(filename.begin(), filename.end(), filename.begin(), std::toupper);
	std::string extension = file::getExtension(filename);
	for (std::string& ext : sub_extensions)
	{
		if (!extension.compare(ext))
			return SCE_TRUE;
	}
	return SCE_FALSE;
};

file::FileEntry::FileEntry(std::string filename, std::string filepath, SceDateTime creationTime, SceBool directory)
{
	_name = filename;
	_path = filepath;
	_creationTime = creationTime;
	_directory = directory;
}

file::FileEntry::~FileEntry(void)
{
}

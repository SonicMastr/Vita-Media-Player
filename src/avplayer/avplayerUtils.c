#include <gxm.h>
#include <display.h>
#include <kernel.h>
#include <math.h>
#include <utils.h>
#include <string.h>
#include <scetypes.h>
#include <vita2d_sys.h>

#define _USE_CDRAM 1

void *memory = NULL;

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

	vita2d_wait_rendering_done();

	ret = sceGxmUnmapMemory(mem);

	ret = sceKernelFreeMemBlock(uid);
}

void* AllocateTexture(void* jumpback, uint32_t alignment, uint32_t size)
{
	SceUID textureMemoryUID;

	return avTextureAlloc(
#ifdef _USE_CDRAM
						SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA,
#else
						SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW,
#endif
						size, alignment,
						SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
						&textureMemoryUID);
};

void DeallocateTexture(void* jumpback, void* pMemory)
{
	avTextureFree(pMemory, SCE_UID_INVALID_UID);
};

void* Allocate(void* jumpback, uint32_t alignment, uint32_t size)
{
	return memalign(alignment, size);
};

void Deallocate(void* jumpback, void* pMemory)
{
	free(pMemory);
};

SceUID FileHandle;

int OpenFile(void* jumpback, const char* argFilename)
{
	FileHandle = sceIoOpen(argFilename, SCE_O_RDONLY, 0);
	return (int)FileHandle;
}

int CloseFile(void* jumpback)
{
	return sceIoClose(FileHandle);
}

int ReadOffsetFile(void* jumpback, uint8_t* argBuffer, uint64_t argPosition, uint32_t argLength)
{
	int64_t ActualRead = 0;

		ActualRead = sceIoPread(FileHandle, argBuffer, argLength, argPosition);

		if (ActualRead < 0)
			return -1;

		if (ActualRead == argLength)
			return (int)ActualRead;
		else
			return 0;
}

uint64_t SizeFile(void* jumpback)
{
	int64_t filesize;

	if(0>sceIoLseek(FileHandle, 0, SCE_SEEK_SET)) {
		return 0;
	}

	filesize = sceIoLseek(FileHandle, 0, SCE_SEEK_END);
	if (0>filesize) {
		return 0;
	}
	if(0>sceIoLseek(FileHandle, 0, SCE_SEEK_SET)) {
		return 0;
	}
	return (uint64_t)filesize;
}

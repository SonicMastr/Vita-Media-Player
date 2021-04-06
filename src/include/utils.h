#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <scetypes.h>
#include <kernel.h>

#define ROUND_UP(x, a)	((((unsigned int)x)+((a)-1u))&(~((a)-1u)))

int readControls(void);
int getEnterButton(void);
int getCancelButton(void);
void setMax(int *set, int value, int max);
void setMin(int *set, int value, int min);
int initAppUtil(void);
int termAppUtil(void);
void* AllocateTexture(void* jumpback, uint32_t alignment, uint32_t size);
void DeallocateTexture(void* jumpback, void* pMemory);
void* Allocate(void* jumpback, uint32_t alignment, uint32_t size);
void Deallocate(void* jumpback, void* pMemory);
int OpenFile(void* jumpback, const char* argFilename);
int CloseFile(void* jumpback);
int ReadOffsetFile(void* jumpback, uint8_t* argBuffer, uint64_t argPosition, uint32_t argLength);
uint64_t SizeFile(void* jumpback);
#endif
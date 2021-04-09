#ifndef AVSUBS_H
#define AVSUBS_H

typedef struct SrtStructure {
    int index;
    uint64_t startTime;
    uint64_t endTime;
    char dialog[1024];
    struct SrtStructure *next;
} SrtStructure_t;

SceInt avOpenSrt(const char *filename);
char *avGetSrt(SceBool *reset);
void *avFreeSrt(void);

#endif

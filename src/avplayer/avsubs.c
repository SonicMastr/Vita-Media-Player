/** Welcome to hell **/
#include "avplayer.h"
#include "common.h"
#include "avsubs.h"
#include "fs.h"

FILE *srtFile = NULL;

uint64_t endTime, startTime = 0;

SrtStructure_t *srtSubtitlesHead, *srtSubtitlesCurrent = NULL;

char *srtDelim = " --> "; // Most if not all SRT should have this for time
char *srtTimeFormat = "%d:%d:%d,%d"; // SRT Time format

char srtText[2048] = {'\0'}; // Actual Text Data

static uint64_t avGetMilliseconds(char *time)
{
    int hours, mins, seconds, milliseconds;

    sscanf(time, srtTimeFormat, &hours, &mins, &seconds, &milliseconds);

    printf("Hours %d\nMins %d\nSecs %d\nMilliseconds %d\n", hours, mins, seconds, milliseconds);

    return hours * 3600000 + mins * 60000 + seconds * 1000 + milliseconds;
}

static char *avCleanSrtDialog(char *dialog)
{
    char *tempChar = strstr(dialog, "</");
    char *out;
    if (tempChar) { // Found XML data
        strncpy(tempChar, dialog, strlen(dialog) - strlen(tempChar));
        if (strstr(tempChar, "an8}")) {
            out = (strrchr(tempChar, '}') + 1); // Find the last tag
        } else {
            out = (strrchr(tempChar, '>') + 1); // Find the last tag
        }
    }
    else
        strncpy(out, dialog, strlen(dialog));
    if (strchr(out, '\r')) {
        int len = strlen(out) + 1;
        for(int i = 0; i<len ; i++)
        {
            if(out[i] == '\r')
                strncpy(&out[i] ,&out[i + 1], len - i);
        }
    }
    return out;
}

static SceVoid avParseSrt()
{
    if (srtFile) {
        srtSubtitlesHead = (SrtStructure_t *)malloc(sizeof(SrtStructure_t));
        srtSubtitlesCurrent = srtSubtitlesHead;
        int curIndex = 0;
        char buf[1024]; // line buffer
        while(fgets(buf, 1024, srtFile)) { // New line
            srtSubtitlesCurrent->next = NULL;
            char *timeLine = strstr(buf, srtDelim); // Check to see if current line contains timestamp data
            if (timeLine) {
                char *start = strtok(buf, srtDelim);
                printf("Start Text: %s\n", start);
                char *end = strtok(NULL, srtDelim);
                printf("End Text: %s\n", end);

                srtSubtitlesCurrent->startTime = avGetMilliseconds(start);
                srtSubtitlesCurrent->endTime = avGetMilliseconds(end);

                printf("Start Time %llu\nEnd Time %llu\nStart Text: %s\nEnd Text: %s\n", srtSubtitlesCurrent->startTime, srtSubtitlesCurrent->endTime, start, end);

                memset(srtText, '\0', sizeof(srtText));
                while (fgets(buf, 1024, srtFile)) {
                    if (strlen(buf) == 1)
                        break;
                    strcat(srtText, buf);
                }
                char *tempText = avCleanSrtDialog(srtText);
                strncpy(srtSubtitlesCurrent->dialog, tempText, strlen(tempText));
                srtSubtitlesCurrent->index = curIndex;
                srtSubtitlesCurrent->next = (SrtStructure_t *)malloc(sizeof(SrtStructure_t));
                srtSubtitlesCurrent = srtSubtitlesCurrent->next;
                curIndex++;
            }
        }
        srtSubtitlesCurrent = srtSubtitlesHead;
    }
}

SceInt avOpenSrt(const char *filename)
{
    int len = strlen(filename);
    const char *extension = &filename[len-4];
    if (!strncasecmp(extension, ".mp4", 4)) {
        char srtPath[1024];
        memset(srtPath, '\0', 1024);
        strncpy(srtPath, filename, len-4);
        strncat(srtPath, ".srt", 4);
        printf("Finding File %s\n\n", srtPath);
        srtFile = fopen(srtPath, "rb");
        if (srtFile) {
            avParseSrt();
            printf("FOUND\n\n", srtPath);
            fclose(srtFile);
            return SCE_OK;
        }
    }
    return -1;
}

char *avGetSrt(SceBool *reset)
{
    if (srtSubtitlesHead) {
        if (*reset) { // Fast Forward and Rewind needs to keep track of this
            srtSubtitlesCurrent = srtSubtitlesHead;
            *reset = 0;
        }
        if (playerTime > srtSubtitlesCurrent->endTime) {
            while (srtSubtitlesCurrent->next) {
                srtSubtitlesCurrent = srtSubtitlesCurrent->next;
                if (srtSubtitlesCurrent->endTime && playerTime < srtSubtitlesCurrent->endTime) {
                    if (playerTime < srtSubtitlesCurrent->startTime) // If we don't put this here, everything will flip
                        return '\0';
                    return srtSubtitlesCurrent->dialog;
                }
            }
            srtSubtitlesCurrent = srtSubtitlesHead;
        }
        if (playerTime < srtSubtitlesCurrent->startTime) // If we don't put this here, everything will flip
            return '\0';
        return srtSubtitlesCurrent->dialog;
    }
    return '\0';
}

void *avFreeSrt()
{
   while (srtSubtitlesHead != NULL)
    {
       srtSubtitlesCurrent = srtSubtitlesHead;
       srtSubtitlesHead = srtSubtitlesHead->next;
       free(srtSubtitlesCurrent);
    }
}

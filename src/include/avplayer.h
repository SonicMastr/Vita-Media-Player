#ifndef AVPLAYER_H
#define AVPLAYER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <scetypes.h>
#include <sceavplayer.h>

int avPlayerInit(void);
int avPlayerTerm(void);
int startPlayback(char *filename);

extern uint64_t playerTime;

#endif
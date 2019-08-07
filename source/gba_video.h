//
//video memory setup
// BTW, remember to learn more about this later...

#ifndef GBA_VIDEO_H
#define GBA_VIDEO_H

#include "gba_types.h"
#include "gba_regs.h"

//Define the screen width and height to be used

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

#define MODE_0 0x0 //Screen Mode 0 (duh)
#define MODE_1 0x1 //Screen Mode 1
#define MODE_2 0x2 //Screen Mode 2
#define MODE_3 0x3 //Screen Mode 3
#define MODE_4 0x4 //Screen Mode 4
#define MODE_5 0x5 //Screen Mode 5

#define BackBuffer 0x10

#define H_BLANK_OAM 0x20

#define OBJ_MAP_2D 0x00
#define OBJ_MAP_1D 0x40

#define BG0_ENABLE 0x100
#define BG1_ENABLE 0x200
#define BG2_ENABLE 0x400
#define BG3_ENABLE 0x800
#define OBJ_ENABLE 0x1000

#define SetMode(mode) REG_DISPCNT = (mode)

#endif

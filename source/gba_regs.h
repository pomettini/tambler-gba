//
//Registers
// (yay!)

#ifndef GBA_REGS_H
#define GBA_REGS_H

#include "gba_types.h"

#define OAM_Mem ((u16 *)0x7000000)
#define OBJ_PaletteMem ((u16 *)0x5000200)
#define OAM_Data ((u16 *)0x6010000)

#define FrontBuffer ((u16 *)0x6000000)
#define BG_PaletteMem ((u16 *)0x5000000)

#define REG_DISPCNT *(u16 *)0x4000000
#define REG_VCOUNT *(volatile u16 *)0x4000006

#define WaitForVsync()        \
    while (REG_VCOUNT != 160) \
        ;

#endif

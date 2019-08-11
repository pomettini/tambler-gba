#ifndef BACKGROUND_H
#define BACKGROUND_H

#define REG_BG0HOFS *(u16 *)0x4000010 //Background 0 Horizontal Offset
#define REG_BG0VOFS *(u16 *)0x4000012 //Background 0 Vertical Offset
#define REG_BG1HOFS *(u16 *)0x4000014 //Background 1 Horizontal Offset
#define REG_BG1VOFS *(u16 *)0x4000016 //Background 1 Vertical Offset
#define REG_BG2HOFS *(u16 *)0x4000018 //Background 2 Horizontal Offset
#define REG_BG2VOFS *(u16 *)0x400001A //Background 2 Vertical Offset
#define REG_BG3HOFS *(u16 *)0x400001C //Background 3 Horizontal Offset
#define REG_BG3VOFS *(u16 *)0x400001E //Background 3 Vertical Offset

#define REG_BG3CNT *(u16 *)0x400000E

u16 *BGTileMem = (u16 *)0x6004000;

#define BG_PRIORITY(x) x
#define CHAR_BASE(n) n << 2
#define SCREEN_BASE(n) n << 8

#define BG_MOSAIC_ENABLE 0x40
#define BG_COLOR_256 0x80
#define BG_COLOR_16 0x0

#define TEXTBG_SIZE_256x256 0x0
#define TEXTBG_SIZE_512x256 0x4000
#define TEXTBG_SIZE_256x512 0x8000
#define TEXTBG_SIZE_512x512 0xC000

#define ROTBG_SIZE_128x128 0x0
#define ROTBG_SIZE_256x256 0x4000
#define ROTBG_SIZE_512x512 0x8000
#define ROTBG_SIZE_1024x1024 0xC000

#define WRAPAROUND 0x2000

typedef struct BG
{
	int x, y, n;
} BG;

//updates the horizontal offset (HOFS) and vertical offset (VOFS) from BG
void UpdateBG(BG *bg)
{
	switch (bg->n)
	{
	case 0:
		REG_BG0HOFS = bg->x;
		REG_BG0VOFS = bg->y;
		break;
	case 1:
		REG_BG1HOFS = bg->x;
		REG_BG1VOFS = bg->y;
		break;
	case 2:
		REG_BG2HOFS = bg->x;
		REG_BG2VOFS = bg->y;
		break;
	case 3:
		REG_BG3HOFS = bg->x;
		REG_BG3VOFS = bg->y;
		break;
	default:
		break;
	}
}

#endif

/*****************************\
* 	background.h
*	Original by dovoto
*	Modified by staringmonkey
*	Last modified on 2/9/02
\*****************************/

#ifndef BACKGROUND_H
#define BACKGROUND_H

#define BIT00 1
#define BIT01 2
#define BIT02 4
#define BIT03 8
#define BIT04 16
#define BIT05 32
#define BIT06 64
#define BIT07 128
#define BIT08 256
#define BIT09 512
#define BIT10 1024
#define BIT11 2048
#define BIT12 4096
#define BIT13 8192
#define BIT14 16384
#define BIT15 32768

////////////////////////////////////Defines//////////////////////////////
#define BG_PRIORITY(x) x
#define CHAR_MEM(x) x << 2
#define SCREEN_MEM(x) x << 8
#define BG_MOSAIC_ENABLE BIT06
#define BG_COLOR_256 BIT07
#define BG_COLOR_16 0x0
#define TEXTBG_SIZE_256x256 0
#define TEXTBG_SIZE_512x256 BIT14
#define TEXTBG_SIZE_256x512 BIT15
#define TEXTBG_SIZE_512x512 BIT14 | BIT15
#define ROTBG_SIZE_128x128 0
#define ROTBG_SIZE_256x256 BIT14
#define ROTBG_SIZE_512x512 BIT15
#define ROTBG_SIZE_1024x1024 BIT14 | BIT15
#define WRAPAROUND BIT13

////////////////////////////////Memory Locations/////////////////////////
//Tile memory sections
u16 *CharMem0 = (u16 *)0x6000000;
u16 *CharMem1 = (u16 *)0x6004000;
u16 *CharMem2 = (u16 *)0x6008000;
u16 *CharMem3 = (u16 *)0x600C000;

//Map memory sections
u16 *ScreenMem0 = (u16 *)0x6000000;
u16 *ScreenMem1 = (u16 *)0x6000800;
u16 *ScreenMem2 = (u16 *)0x6001000;
u16 *ScreenMem3 = (u16 *)0x6001800;
u16 *ScreenMem4 = (u16 *)0x6002000;
u16 *ScreenMem5 = (u16 *)0x6002800;
u16 *ScreenMem6 = (u16 *)0x6003000;
u16 *ScreenMem7 = (u16 *)0x6003800;
u16 *ScreenMem8 = (u16 *)0x6004000;
u16 *ScreenMem9 = (u16 *)0x6004800;
u16 *ScreenMem10 = (u16 *)0x6005000;
u16 *ScreenMem11 = (u16 *)0x6005800;
u16 *ScreenMem12 = (u16 *)0x6006000;
u16 *ScreenMem13 = (u16 *)0x6006800;
u16 *ScreenMem14 = (u16 *)0x6007000;
u16 *ScreenMem15 = (u16 *)0x6007800;
u16 *ScreenMem16 = (u16 *)0x6008000;
u16 *ScreenMem17 = (u16 *)0x6008800;
u16 *ScreenMem18 = (u16 *)0x6009000;
u16 *ScreenMem19 = (u16 *)0x6009800;
u16 *ScreenMem20 = (u16 *)0x600A000;
u16 *ScreenMem21 = (u16 *)0x600A800;
u16 *ScreenMem22 = (u16 *)0x600B000;
u16 *ScreenMem23 = (u16 *)0x600B800;
u16 *ScreenMem24 = (u16 *)0x600C000;
u16 *ScreenMem25 = (u16 *)0x600C800;
u16 *ScreenMem26 = (u16 *)0x600D000;
u16 *ScreenMem27 = (u16 *)0x600D800;
u16 *ScreenMem28 = (u16 *)0x600E000;
u16 *ScreenMem29 = (u16 *)0x600E800;
u16 *ScreenMem30 = (u16 *)0x600F000;
u16 *ScreenMem31 = (u16 *)0x600F800;

///////////////////////////////Function Prototypes///////////////////////
void LoadBackgroundTiles(u16 *source, u8 charMem, u16 amount);
void LoadBackgroundMap(u16 *source, u8 screenMem, u16 width, u16 height);
void FillBackgroundMap(u8 screenMem, u8 width, u8 height, u8 fillValue);

///////////////////////////////LoadBackgroundTiles///////////////////////
void LoadBackgroundTiles(u16 *source, u8 charMem, u16 amount)
{
	//Looping variable
	u16 i;

	//Where to put tiles
	u16 *destination;

	//Switch to correct CharMem block
	switch (charMem)
	{
	case 0:
		destination = CharMem0;
		break;
	case 1:
		destination = CharMem1;
		break;
	case 2:
		destination = CharMem2;
		break;
	case 3:
		destination = CharMem3;
		break;
	}

	//Load the tiles into background memory
	for (i = 0; i < amount; i++)
	{
		destination[i] = source[i];
	}
}

///////////////////////////////LoadBackgroundTiles///////////////////////
void LoadBackgroundMap(u16 *source, u8 screenMem, u16 width, u16 height)
{
	//Looping variable
	u16 x, y;

	//Where to put tiles
	u16 *destination;

	//Switch to correct ScreenMem block
	switch (screenMem)
	{
	case 0:
		destination = ScreenMem0;
		break;
	case 1:
		destination = ScreenMem1;
		break;
	case 2:
		destination = ScreenMem2;
		break;
	case 3:
		destination = ScreenMem3;
		break;
	case 4:
		destination = ScreenMem4;
		break;
	case 5:
		destination = ScreenMem5;
		break;
	case 6:
		destination = ScreenMem6;
		break;
	case 7:
		destination = ScreenMem7;
		break;
	case 8:
		destination = ScreenMem8;
		break;
	case 9:
		destination = ScreenMem9;
		break;
	case 10:
		destination = ScreenMem10;
		break;
	case 11:
		destination = ScreenMem11;
		break;
	case 12:
		destination = ScreenMem12;
		break;
	case 13:
		destination = ScreenMem13;
		break;
	case 14:
		destination = ScreenMem14;
		break;
	case 15:
		destination = ScreenMem15;
		break;
	case 16:
		destination = ScreenMem16;
		break;
	case 17:
		destination = ScreenMem17;
		break;
	case 18:
		destination = ScreenMem18;
		break;
	case 19:
		destination = ScreenMem19;
		break;
	case 20:
		destination = ScreenMem20;
		break;
	case 21:
		destination = ScreenMem21;
		break;
	case 22:
		destination = ScreenMem22;
		break;
	case 23:
		destination = ScreenMem23;
		break;
	case 24:
		destination = ScreenMem24;
		break;
	case 25:
		destination = ScreenMem25;
		break;
	case 26:
		destination = ScreenMem26;
		break;
	case 27:
		destination = ScreenMem27;
		break;
	case 28:
		destination = ScreenMem28;
		break;
	case 29:
		destination = ScreenMem29;
		break;
	case 30:
		destination = ScreenMem30;
		break;
	case 31:
		destination = ScreenMem31;
		break;
	}

	//Load the tiles into background memory
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			destination[y * width + x] = source[y * width + x];
		}
	}
}

//////////////////////////////////////ClearBackground//////////////////////////////
void FillBackgroundMap(u8 screenMem, u8 width, u8 height, u8 fillValue)
{
	//Looping variables
	u16 x, y;

	//What layer to clear
	u16 *destination;

	//Switch to correct ScreenMem block
	switch (screenMem)
	{
	case 0:
		destination = ScreenMem0;
		break;
	case 1:
		destination = ScreenMem1;
		break;
	case 2:
		destination = ScreenMem2;
		break;
	case 3:
		destination = ScreenMem3;
		break;
	case 4:
		destination = ScreenMem4;
		break;
	case 5:
		destination = ScreenMem5;
		break;
	case 6:
		destination = ScreenMem6;
		break;
	case 7:
		destination = ScreenMem7;
		break;
	case 8:
		destination = ScreenMem8;
		break;
	case 9:
		destination = ScreenMem9;
		break;
	case 10:
		destination = ScreenMem10;
		break;
	case 11:
		destination = ScreenMem11;
		break;
	case 12:
		destination = ScreenMem12;
		break;
	case 13:
		destination = ScreenMem13;
		break;
	case 14:
		destination = ScreenMem14;
		break;
	case 15:
		destination = ScreenMem15;
		break;
	case 16:
		destination = ScreenMem16;
		break;
	case 17:
		destination = ScreenMem17;
		break;
	case 18:
		destination = ScreenMem18;
		break;
	case 19:
		destination = ScreenMem19;
		break;
	case 20:
		destination = ScreenMem20;
		break;
	case 21:
		destination = ScreenMem21;
		break;
	case 22:
		destination = ScreenMem22;
		break;
	case 23:
		destination = ScreenMem23;
		break;
	case 24:
		destination = ScreenMem24;
		break;
	case 25:
		destination = ScreenMem25;
		break;
	case 26:
		destination = ScreenMem26;
		break;
	case 27:
		destination = ScreenMem27;
		break;
	case 28:
		destination = ScreenMem28;
		break;
	case 29:
		destination = ScreenMem29;
		break;
	case 30:
		destination = ScreenMem30;
		break;
	case 31:
		destination = ScreenMem31;
		break;
	}

	//Clear it out
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			destination[y * width + x] = fillValue;
		}
	}
}

#endif

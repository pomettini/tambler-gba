#include <string.h>
#include "gba.h"
#include "sprites.h"

#define FALSE 0
#define TRUE 1

#define ever \
	;        \
	;

enum states
{
	MENU = 0,
	TUTORIAL = 1,
	GAME = 2,
	GAME_OVER = 3
};

int game_state = MENU;

OAMEntry sprites[128];

u16 pos_x = 0;
u16 pos_y = 0;
u16 palette = 0;

void CopyOAM();
void InitializeSprites();
void MoveSprite(OAMEntry *sp, int x, int y);

int main()
{
	u16 loop;

	SetMode(MODE_1 | OBJ_ENABLE | OBJ_MAP_2D);

	for (loop = 0; loop < 256; loop++)
		OBJ_PaletteMem[loop] = tileset_palette[loop];

	InitializeSprites();

	memcpy(OAM_Data, tileset_data, sizeof(tileset_data));

	// LoadBackgroundTiles(lettersData, 2, letters_WIDTH * letters_HEIGHT / 2);

	for (;;)
	{
		if (keyDown(KEY_L))
			palette--;

		if (keyDown(KEY_R))
			palette++;

		if (keyDown(KEY_LEFT))
			pos_x--;

		if (keyDown(KEY_RIGHT))
			pos_x++;

		if (keyDown(KEY_UP))
			pos_y--;

		if (keyDown(KEY_DOWN))
			pos_y++;

		sprites[0].attribute0 = COLOR_256 | SQUARE | pos_y;
		sprites[0].attribute1 = SIZE_64 | pos_x;
		sprites[0].attribute2 = palette;

		// sprites[1].attribute0 = COLOR_256 | WIDE | 30 + pos_y;
		// sprites[1].attribute1 = SIZE_32 | 30 + pos_x;
		// sprites[1].attribute2 = 2;

		// WriteText(21, 30, 18, 19, "Pause", 10, 0, 0);

		WaitForVsync();
		CopyOAM();
	}

	return 0;
}

void CopyOAM()
{
	u16 loop;
	u16 *temp;
	temp = (u16 *)sprites;
	for (loop = 0; loop < 128 * 4; loop++)
	{
		OAM_Mem[loop] = temp[loop];
	}
}

void InitializeSprites()
{
	for (int x = 0; x < 128; x++)
	{
		sprites[x].attribute0 = 160; //y > 159
		sprites[x].attribute1 = 240; //x > 239
	}
}

void MoveSprite(OAMEntry *sp, int x, int y)
{
	if (x < 0) //if it is off the left correct
		x = 512 + x;
	if (y < 0) //if off the top correct
		y = 256 + y;

	sp->attribute1 = sp->attribute1 & 0xFE00; //clear the old x value
	sp->attribute1 = sp->attribute1 | x;

	sp->attribute0 = sp->attribute0 & 0xFF00; //clear the old y value
	sp->attribute0 = sp->attribute0 | y;
}
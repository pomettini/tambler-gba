#include <string.h>
#include "gba.h"
#include "sprites.h"
#include "main.h"

// States
u16 game_state = GAME;

// Gameplay globals
database_t database[100];
post_t posts[4];
u16 score = 0;
u16 countdown = COUNTDOWN_START_VAL;

// Animation globals
u16 menu_lock = FALSE;

// Game animation globals
s16 swipe_direction = 0;
u16 is_animating_feed = FALSE;
u16 is_animating_swipe = FALSE;
s16 posts_y_offset = 0;
s16 posts_x_offset = 0;

// Const globals
const u16 countdown_speed = 1;

// GBA stuff
OAMEntry sprites[128];

u16 pos_x = 0;
u16 pos_y = 0;
u16 palette = 0;

int main()
{
	u16 loop;

	SetMode(MODE_1 | OBJ_ENABLE | OBJ_MAP_2D);

	for (loop = 0; loop < 256; loop++)
		OBJ_PaletteMem[loop] = tileset_palette[loop];

	InitializeSprites();
	ResetSpritesPosition();

	memcpy(OAM_Data, tileset_data, sizeof(tileset_data));

	// for (;;)
	// {
	// 	MoveMediumSprite(MEDIUM_SPR_START_OFFSET, 16, 16);
	// 	MoveMediumSprite(MEDIUM_SPR_START_OFFSET + 1, 16, 64);
	// 	MoveMediumSprite(MEDIUM_SPR_START_OFFSET + 2, 16, 112);

	// 	MoveBigSprite(BIG_SPR_START_OFFSET, 48, 16);
	// 	MoveBigSprite(BIG_SPR_START_OFFSET + 1, 48, 64);
	// 	MoveBigSprite(BIG_SPR_START_OFFSET + 2, 48, 112);

	// 	WaitForVsync();
	// 	CopyOAM();
	// }

	for (ever)
	{
		_Update();
		_Draw();
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

void ResetSpritesPosition()
{
	for (int x = 0; x < 128; x++)
	{
		sprites[x].attribute0 = 160; //y > 159
		sprites[x].attribute1 = 240; //x > 239
	}
}

void InitializeSprites()
{
	// Init small sprites
	for (u16 i = 0; i < 16; i++)
	{
		sprites[i].attribute0 = COLOR_256 | SQUARE | i * 8;
		sprites[i].attribute1 = SIZE_8 | i * 8;
		sprites[i].attribute2 = 2 + (i * 2);
	}

	// Init medium sprites
	for (u16 i = 0; i < 8; i++)
	{
		sprites[i + 16].attribute0 = COLOR_256 | SQUARE | i * 16;
		sprites[i + 16].attribute1 = SIZE_16 | i * 16;
		sprites[i + 16].attribute2 = 64 + (i * 4);
	}

	// Init big sprites
	for (u16 i = 0; i < 4; i++)
	{
		sprites[i + 24].attribute0 = COLOR_256 | SQUARE | i * 32;
		sprites[i + 24].attribute1 = SIZE_32 | i * 32;
		sprites[i + 24].attribute2 = 128 + (i * 8);
	}
}

void MoveSmallSprite(u16 id, u16 pos_x, u16 pos_y)
{
	sprites[id].attribute0 = COLOR_256 | SQUARE | pos_y;
	sprites[id].attribute1 = SIZE_8 | pos_x;
}

void MoveMediumSprite(u16 id, u16 pos_x, u16 pos_y)
{
	sprites[id].attribute0 = COLOR_256 | SQUARE | pos_y;
	sprites[id].attribute1 = SIZE_16 | pos_x;
}

void MoveBigSprite(u16 id, u16 pos_x, u16 pos_y)
{
	sprites[id].attribute0 = COLOR_256 | SQUARE | pos_y;
	sprites[id].attribute1 = SIZE_32 | pos_x;
}

void LoadDatabase()
{
}

void LoadTutorialPost()
{
}

void GeneratePosts()
{
}

void PopAndPushPost()
{
}

void PopAndPushTutorial()
{
}

void ProcessButtons()
{
}

void ProcessFeedAnimation()
{
}

void ProcessSwipeAnimation()
{
	if (is_animating_swipe == TRUE)
	{
		posts_x_offset += 16 * swipe_direction;
	}

	if (posts_x_offset <= -160 || posts_x_offset >= 160)
	{
		is_animating_feed = TRUE;
		is_animating_swipe = FALSE;
	}
}

void DecreaseCountdown()
{
	if (is_animating_swipe == FALSE)
	{
		countdown -= countdown_speed * (score + 1);
	}
}

void PostAnimationEnded()
{
	if (game_state == TUTORIAL)
	{
		// pop and push tutorial
	}

	if (game_state == GAME)
	{
		// evaluate content
		// pop and push post
	}
}

void EvaluateContent(post_t *post)
{
	if (post->is_valid == TRUE && swipe_direction == 1 || post->is_valid == FALSE && swipe_direction == -1)
	{
		score++;
		countdown = COUNTDOWN_START_VAL;
	}
	else
	{
		countdown = 0;
		// sfx wrong
	}
}

void EvaluateGameOver()
{
	if (countdown <= 0)
	{
		ChangeState(GAME_OVER);
		// sfx wrong
	}
}

void ChangeState(u16 new_state)
{
	menu_lock = TRUE;
	game_state = new_state;

	if (new_state == GAME)
	{
		ResetGameState();
		StartGameMusic();
	}
}

void ResetGameState()
{
	LoadDatabase();
	GeneratePosts();
	score = 0;
	countdown = COUNTDOWN_START_VAL;
	swipe_direction = 0;
	is_animating_feed = FALSE;
	is_animating_swipe = FALSE;
	posts_x_offset = 0;
	posts_y_offset = 0;
}

void StartGameMusic()
{
}

void _Init()
{
}

void _Update()
{
	if (game_state == MENU)
	{
	}
	else if (game_state == TUTORIAL)
	{
	}
	else if (game_state == GAME)
	{
	}
	else if (game_state == GAME_OVER)
	{
	}
	else if (game_state == CREDITS)
	{
	}
}

void _Draw()
{
	if (game_state == MENU)
	{
	}
	else if (game_state == TUTORIAL)
	{
	}
	else if (game_state == GAME)
	{
	}
	else if (game_state == GAME_OVER)
	{
	}
	else if (game_state == CREDITS)
	{
	}
}
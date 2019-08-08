#include <string.h>
#include <stdlib.h>
#include "gba.h"
#include "sprites.h"
#include "main.h"

// States
u16 game_state = GAME;

// Gameplay globals
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
	SetMode(MODE_1 | OBJ_ENABLE | OBJ_MAP_2D);

	for (u16 i = 0; i < 256; i++)
		OBJ_PaletteMem[i] = tileset_palette[i];

	InitializeSprites();
	ResetSpritesPosition();

	memcpy(OAM_Data, tileset_data, sizeof(tileset_data));

	for (ever)
	{
		_Update();
		_Draw();
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

void MoveSmallSprite(u16 id, s16 pos_x, s16 pos_y)
{
	if (pos_x < 0)
		pos_x = 256;

	sprites[id].attribute0 = COLOR_256 | SQUARE | pos_y;
	sprites[id].attribute1 = SIZE_8 | pos_x;
}

void MoveMediumSprite(u16 id, s16 pos_x, s16 pos_y)
{
	if (pos_x < 0)
		pos_x = 256;

	sprites[id].attribute0 = COLOR_256 | SQUARE | pos_y;
	sprites[id].attribute1 = SIZE_16 | pos_x;
}

void MoveBigSprite(u16 id, s16 pos_x, s16 pos_y)
{
	if (pos_x < 0)
		pos_x = 256;

	sprites[id].attribute0 = COLOR_256 | SQUARE | pos_y;
	sprites[id].attribute1 = SIZE_32 | pos_x;
}

void LoadTutorialPost()
{
}

void GeneratePosts()
{
	// for (u16 i = 0; i < 4; i++)
	// {
	// 	posts[i] =
	// }
}

void PopAndPushPost()
{
	// I need to pop post after the animation finishes
	posts[0] = posts[1];
	posts[1] = posts[2];
	posts[2] = posts[3];
	// posts[3] = database[GetRandomPostId()];
}

void PopAndPushTutorial()
{
}

post_t GetRandomPost()
{
	return DATABASE[rand() % POSTS_NUM];
}

u16 GetRandomProfilePic()
{
	return MEDIUM_SPR_START_OFFSET + (rand() % MEDIUM_SPR_NUM);
}

u16 GetRandomPostPic()
{
	return BIG_SPR_START_OFFSET + (rand() % BIG_SPR_NUM);
}

void ProcessButtons()
{
	if (is_animating_swipe == FALSE && menu_lock == FALSE)
	{
		if (keyDown(KEY_LEFT))
		{
			swipe_direction = -1;
			is_animating_swipe = TRUE;
			// sfx correct
			menu_lock = TRUE;
		}

		if (keyDown(KEY_RIGHT))
		{
			swipe_direction = 1;
			is_animating_swipe = TRUE;
			// sfx correct
			menu_lock = TRUE;
		}
	}
	else
	{
		menu_lock = FALSE;
	}
}

void ProcessFeedAnimation()
{
	if (is_animating_feed == TRUE)
	{
		posts_y_offset -= 10;
	}

	if (posts_y_offset <= -40)
	{
		is_animating_feed = FALSE;
		// post animation ended
		posts_x_offset = 0;
		posts_y_offset = 0;
	}
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
	GeneratePosts();
	score = 0;
	countdown = COUNTDOWN_START_VAL;
	swipe_direction = 0;
	is_animating_feed = FALSE;
	is_animating_swipe = FALSE;
	posts_x_offset = 0;
	posts_y_offset = 0;
}

void DrawPost(s16 x_offset, s16 y_offset, post_t *post)
{
	// Post bg
	// Profile pic
	MoveMediumSprite(post->profile_id, 8 + x_offset, 8 + y_offset);
	// Image
	MoveBigSprite(post->pic_id, 32 + x_offset, 8 + y_offset);
	// Text
}

void DrawPosts()
{
	s16 y_offset = posts_y_offset + 16;
	s16 x_offset = posts_x_offset;

	MoveMediumSprite(MEDIUM_SPR_START_OFFSET, 16 + x_offset, 0 + y_offset);
	MoveMediumSprite(MEDIUM_SPR_START_OFFSET + 1, 16, 40 + y_offset);
	MoveMediumSprite(MEDIUM_SPR_START_OFFSET + 2, 16, 80 + y_offset);
	MoveMediumSprite(MEDIUM_SPR_START_OFFSET + 3, 16, 120 + y_offset);

	MoveBigSprite(BIG_SPR_START_OFFSET, 48 + x_offset, 0 + y_offset);
	MoveBigSprite(BIG_SPR_START_OFFSET + 1, 48, 40 + y_offset);
	MoveBigSprite(BIG_SPR_START_OFFSET + 2, 48, 80 + y_offset);
	MoveBigSprite(BIG_SPR_START_OFFSET + 3, 48, 120 + y_offset);
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
		ProcessButtons();
		ProcessFeedAnimation();
		ProcessSwipeAnimation();
		DecreaseCountdown();
		// EvaluateGameOver();
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
		DrawPosts();
	}
	else if (game_state == GAME_OVER)
	{
	}
	else if (game_state == CREDITS)
	{
	}
}
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gba.h"
#include "sprites.h"
#include "font.h"
#include "game_background.h"
#include "main.h"

// States
u16 game_state = GAME;

// Gameplay globals
post_t posts[4];
u16 score = 0;
s16 countdown = COUNTDOWN_START_VAL;

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
const u16 text_y_offset = -3;

// GBA stuff
OAMEntry sprites[128];

u16 pos_x = 0;
u16 pos_y = 0;
u16 palette = 0;

u16 current_char = 0;
u16 current_post_id = 0;

#define REG_BG0CNT *(volatile u16 *)0x4000008

int main()
{
	SetMode(MODE_0 | BG0_ENABLE | OBJ_ENABLE | OBJ_MAP_2D);

	// TODO: Find a way to generate a random seed
	srand(10);

	REG_BG0CNT = 0x1F83;

	for (u16 i = 0; i < 256; i++)
	{
		OBJ_PaletteMem[i] = tileset_palette[i];
		BG_PaletteMem[i] = game_background_palette[i];
	}

	memcpy(FrontBuffer, game_background_data, sizeof(game_background_data));
	memcpy(OAM_Data, tileset_data, sizeof(tileset_data));
	// Was 4096
	memcpy(OAM_Data + 8192, font_data, sizeof(font_data));

	InitializeSprites();

	ResetSpritesPosition();

	GeneratePosts();

	for (ever)
	{
		current_char = 0;

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

void DrawCharacter(unsigned char char_, s16 pos_x, s16 pos_y)
{
	if (pos_x < 0)
		pos_x = 256;

	if (current_char >= LETTERS_MAX)
		return;

	sprites[BIG_SPR_START_OFFSET + BIG_SPR_NUM + current_char].attribute0 = COLOR_256 | SQUARE | pos_y;
	sprites[BIG_SPR_START_OFFSET + BIG_SPR_NUM + current_char].attribute1 = SIZE_8 | pos_x;
	sprites[BIG_SPR_START_OFFSET + BIG_SPR_NUM + current_char].attribute2 = 512 + ((int)char_ * 2);

	current_char++;
}

void DrawText(unsigned char *text, s16 pos_x, s16 pos_y)
{
	u16 curr_pos_x = pos_x;
	while (*text != '\0')
	{
		DrawCharacter(*text++, curr_pos_x, pos_y);
		curr_pos_x += 8;
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

void LoadTutorialPosts()
{
}

void GeneratePosts()
{
	for (u16 i = 0; i < 4; i++)
	{
		posts[i] = GeneratePost();
	}
}

post_t GeneratePost()
{
	u16 random_post = GetRandomPostId();
	post_t post = {
		"10 motivi",
		"Per smettere",
		"Di fumare",
		TRUE,
		GetRandomProfilePic(),
		GetRandomPostPic()};
	return post;
}

void PopAndPushPost()
{
	// I need to pop post after the animation finishes
	posts[0] = posts[1];
	posts[1] = posts[2];
	posts[2] = posts[3];
	posts[3] = GeneratePost();
}

void PopAndPushTutorial()
{
}

u16 GetRandomPostId()
{
	return rand() % POSTS_NUM;
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

	if (posts_x_offset <= -240 || posts_x_offset >= 240)
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
	DrawPostBg(current_post_id, x_offset, y_offset);
	// Profile pic
	MoveMediumSprite(post->profile_id, 8 + x_offset, 8 + y_offset);
	// Image
	MoveBigSprite(post->pic_id, 32 + x_offset, 8 + y_offset);
	// Text
	DrawText(
		post->first,
		72 + x_offset,
		16 + y_offset + text_y_offset);
	DrawText(
		post->second,
		72 + x_offset,
		24 + y_offset + text_y_offset);
	DrawText(
		post->third,
		72 + x_offset,
		32 + y_offset + text_y_offset);

	current_post_id += 2;
}

void DrawPosts()
{
	s16 y_offset = posts_y_offset + 16;
	s16 x_offset = posts_x_offset;

	current_post_id = 0;

	DrawPost(20 + x_offset, 0 + y_offset, &posts[0]);
	DrawPost(20, 40 + y_offset, &posts[1]);
	DrawPost(20, 80 + y_offset, &posts[2]);
	// DrawPost(20, 120 + y_offset, &posts[3]);
}

void DrawPostBg(u16 id, s16 pos_x, s16 pos_y)
{
	if (pos_x < -64)
		pos_x = 256;

	sprites[120 + id].attribute0 = COLOR_256 | WIDE | ROTATION_FLAG | SIZE_DOUBLE | 8 + pos_y;
	sprites[120 + id].attribute1 = SIZE_32 | ROTDATA(0) | 64 + pos_x;
	sprites[120 + id].attribute2 = 256 + 128;

	sprites[121 + id].attribute0 = COLOR_256 | WIDE | ROTATION_FLAG | SIZE_DOUBLE | 8 + pos_y;
	sprites[121 + id].attribute1 = SIZE_32 | ROTDATA(0) | 128 + pos_x;
	sprites[121 + id].attribute2 = 256 + 128;
}

void DrawCountdownBar()
{
	sprites[118].attribute0 = COLOR_256 | WIDE | ROTATION_FLAG | SIZE_DOUBLE | 142;
	sprites[118].attribute1 = SIZE_64 | ROTDATA(0) | countdown - 120 - 16;
	sprites[118].attribute2 = 256 + 128;

	sprites[119].attribute0 = COLOR_256 | WIDE | ROTATION_FLAG | SIZE_DOUBLE | 142;
	sprites[119].attribute1 = SIZE_64 | ROTDATA(0) | countdown - 240 - 16;
	sprites[119].attribute2 = 256 + 128;

	// char countdown_str[12];
	// sprintf(countdown_str, "%i", countdown);

	// DrawText(countdown_str, 0, 0);
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
		EvaluateGameOver();
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
		DrawCountdownBar();
	}
	else if (game_state == GAME_OVER)
	{
	}
	else if (game_state == CREDITS)
	{
	}
}
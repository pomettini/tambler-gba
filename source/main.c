#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gba.h"
#include "sprites.h"
#include "font.h"
#include "background.h"
#include "game_bg.h"
#include "title_bg.h"
#include "africa.h"
#include "main.h"

// TODO: Add sfx
// TODO: Add menu music
// TODO: Loop music
// TODO: Add sprite flipping
// TODO: Fix magenta screen between swaps
// TODO: Add falling stuff on menu screen
// TODO: Add small sprites on post images
// TODO: Add best score
// TODO: Split code to multiple files

// States
u16 game_state = MENU;

// Gameplay globals
post_t posts[POST_NUM];
tutorial_post_t tutorial_posts[TUTORIAL_POST_NUM];
u16 score = 0;
s16 countdown = COUNTDOWN_START_VAL;
s16 countdown_ = COUNTDOWN_SECOND_START_VAL;

// Animation globals
u16 menu_lock = FALSE;

// Game animation globals
s16 swipe_direction = 0;
u16 is_animating_feed = FALSE;
u16 is_animating_swipe = FALSE;
s16 posts_y_offset = 0;
s16 posts_x_offset = 0;

// Const globals
const u16 COUNTDOWN_SPEED = 3;
const u16 TEXT_Y_OFFSET = -4;

// GBA stuff
OAMEntry sprites[SPRITE_NUM];

u16 pos_x = 0;
u16 pos_y = 0;
u16 palette = 0;

u16 current_char = 0;
u16 current_char_bg = 0;
u16 current_post_id = 0;

u16 __key_curr = 0, __key_prev = 0;

u16 mosaic_effect = FALSE;
u16 mosaic_amount = 0;
s16 mosaic_delta = 0;

u16 next_state = 0;
u16 counter = 0;

int main()
{
	_Init();

	PlayMusic();

	for (ever)
	{
		current_char = 0;
		counter++;

		KeyPoll();
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

	for (loop = 0; loop < SPRITE_NUM * 4; loop++)
		OAM_Mem[loop] = temp[loop];
}

void ResetSpritesPosition()
{
	for (u16 x = 0; x < SPRITE_NUM; x++)
	{
		sprites[x].attribute0 = SCREEN_HEIGHT;
		sprites[x].attribute1 = SCREEN_WIDTH;
	}
}

void InitializeSprites()
{
	// Init small sprites
	for (u16 i = 0; i < 16; i++)
	{
		sprites[i].attribute0 = COLOR_256 | MOSAIC | SQUARE | i * 8;
		sprites[i].attribute1 = SIZE_8 | i * 8;
		sprites[i].attribute2 = 2 + (i * 2);
	}
}

void DrawCharacter(unsigned char char_, s16 pos_x, s16 pos_y, u16 wave)
{
	if (pos_x < 0)
		pos_x = SCREEN_WIDTH;

	if (current_char >= LETTERS_MAX)
		return;

	if (wave == TRUE)
		sprites[BIG_SPR_START_OFFSET + BIG_SPR_NUM + current_char].attribute0 = COLOR_256 | MOSAIC | SQUARE | pos_y + SIN_LUT[((current_char * 4) + counter) % SIN_LUT_MAX];
	else
		sprites[BIG_SPR_START_OFFSET + BIG_SPR_NUM + current_char].attribute0 = COLOR_256 | MOSAIC | SQUARE | pos_y;

	sprites[BIG_SPR_START_OFFSET + BIG_SPR_NUM + current_char].attribute1 = SIZE_8 | pos_x;
	sprites[BIG_SPR_START_OFFSET + BIG_SPR_NUM + current_char].attribute2 = SPRITE_START_ADDR + 320 + ((u16)char_ * 2);

	// If character is empty, it will not be stored on the sprite table
	if (char_ != ' ')
		current_char++;
}

void DrawText(unsigned char *text, s16 pos_x, s16 pos_y, u16 wave)
{
	u16 curr_pos_x = pos_x;

	while (*text != '\0')
	{
		DrawCharacter(*text, curr_pos_x, pos_y, wave);
		curr_pos_x += 8;
		text++;
	}
}

void DrawCharacterBg(unsigned char char_, s16 pos_x, s16 pos_y)
{
	u16 bitmap_x = (char_ % 64) * 64;
	u16 bitmap_y = (char_ / 64) * 64;

	u16 start_at = (64 * bitmap_y) + bitmap_x;

	u16 stupid_counter = 0;
	for (u16 y = 0; y < 8; y++)
	{
		for (u16 x = 0; x < 8; x++)
		{
			PlotPixel(x + pos_x, y + pos_y, font_data[start_at + stupid_counter]);
			stupid_counter++;
		}
	}
}

void DrawTextBg(unsigned char *text, s16 pos_x, s16 pos_y)
{
	u16 curr_pos_x = pos_x;

	while (*text != '\0')
	{
		DrawCharacterBg(*text, curr_pos_x, pos_y);
		curr_pos_x += 8;
		text++;
	}
}

void CleanCharacterSprites()
{
	for (u16 i = TEXT_SPR_OFFSET + current_char; i < TEXT_SPR_OFFSET + LETTERS_MAX; i++)
	{
		sprites[i].attribute0 = SCREEN_HEIGHT;
		sprites[i].attribute1 = SCREEN_WIDTH;
	}
}

void MoveSmallSprite(u16 id, s16 pos_x, s16 pos_y)
{
	if (pos_x < 0)
		pos_x = SCREEN_WIDTH;

	sprites[id].attribute0 = COLOR_256 | MOSAIC | SQUARE | pos_y;
	sprites[id].attribute1 = SIZE_8 | pos_x;
}

void MoveMediumSprite(u16 id, s16 pos_x, s16 pos_y)
{
	if (pos_x < 0)
		pos_x = SCREEN_WIDTH;

	sprites[id].attribute0 = COLOR_256 | MOSAIC | SQUARE | pos_y;
	sprites[id].attribute1 = SIZE_16 | pos_x;
}

void MoveBigSprite(u16 id, s16 pos_x, s16 pos_y)
{
	if (pos_x < 0)
		pos_x = SCREEN_WIDTH;

	sprites[id].attribute0 = COLOR_256 | MOSAIC | SQUARE | pos_y;
	sprites[id].attribute1 = SIZE_32 | pos_x;
}

void SetSprite(u16 oam_id, u16 sprite_id)
{
	sprites[oam_id].attribute2 = SPRITE_START_ADDR + sprite_id;
}

void KeyPoll()
{
	__key_prev = __key_curr;
	__key_curr = ~KEYS & 0x03FF;
}

u32 KeyReleased(u32 key)
{
	// Generates another random seed
	rand();

	return (~__key_curr & __key_prev) & key;
}

void GeneratePosts()
{
	for (u16 i = 0; i < POST_NUM; i++)
		posts[i] = GeneratePost();
}

void InsertTutorialPosts()
{
	for (u16 i = 0; i < TUTORIAL_POST_NUM; i++)
		tutorial_posts[i] = TUTORIAL_POSTS[i];
}

post_t GeneratePost()
{
	u16 random_id = GetRandomPostId();

	post_t post = {
		"",
		"",
		"",
		DATABASE[random_id].is_valid,
		GetRandomProfilePic(),
		GetRandomPostPic()};

	// TODO: Find a way to do it in struct initialization
	memcpy(
		&post.first,
		DATABASE[random_id].first,
		TEXT_LEN_MAX);
	memcpy(
		&post.second,
		DATABASE[random_id].second,
		TEXT_LEN_MAX);
	memcpy(
		&post.third,
		DATABASE[random_id].third,
		TEXT_LEN_MAX);

	return post;
}

void PopAndPushPost()
{
	// I need to pop post after the animation finishes
	posts[0] = posts[1];
	posts[1] = posts[2];
	posts[2] = GeneratePost();
}

void PopAndPushTutorial()
{
	tutorial_post_t last_post = {"", "", "", TRUE};

	for (u16 i = 0; i < TUTORIAL_POST_NUM - 1; i++)
		tutorial_posts[i] = tutorial_posts[i + 1];

	tutorial_posts[4] = last_post;
}

u16 GetRandomPostId()
{
	return rand() % POSTS_NUM;
}

u16 GetRandomProfilePic()
{
	return 64 + ((rand() % MEDIUM_SPR_NUM) * 4);
}

u16 GetRandomPostPic()
{
	return 128 + ((rand() % BIG_SPR_NUM) * 8);
}

void ProcessButtons()
{
	if (is_animating_swipe == FALSE && menu_lock == FALSE)
	{
		if (KeyReleased(KEY_L))
		{
			swipe_direction = -1;
			is_animating_swipe = TRUE;
			// sfx correct
			menu_lock = TRUE;
		}

		if (KeyReleased(KEY_R))
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
		posts_y_offset -= 10;

	if (posts_y_offset <= -40)
	{
		is_animating_feed = FALSE;
		PostAnimationEnded();
		posts_x_offset = 0;
		posts_y_offset = 0;
	}
}

void ProcessSwipeAnimation()
{
	if (is_animating_swipe == TRUE)
		posts_x_offset += 16 * swipe_direction;

	if (posts_x_offset <= -SCREEN_WIDTH || posts_x_offset >= SCREEN_WIDTH)
	{
		is_animating_feed = TRUE;
		is_animating_swipe = FALSE;
	}
}

void DecreaseCountdown()
{
	countdown_ -= (score + 1);

	if (countdown_ > 0)
		return;
	else
		countdown_ = COUNTDOWN_SECOND_START_VAL;

	if (is_animating_swipe == FALSE)
		countdown -= COUNTDOWN_SPEED;
}

void PostAnimationEnded()
{
	if (game_state == TUTORIAL)
		PopAndPushTutorial();

	if (game_state == GAME)
	{
		EvaluateContent(&posts[0]);
		PopAndPushPost();
	}
}

void EvaluateContent(post_t *post)
{
	if (post->is_valid == TRUE && swipe_direction == 1 ||
		post->is_valid == FALSE && swipe_direction == -1)
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

	switch (new_state)
	{
	case MENU:
		SwapToTitleBg();
		break;
	case TUTORIAL:
		SwapToTutorialBg();
		break;
	case GAME:
		SwapToGameBg();
		SetTextColor(COLOR_BLACK);
		break;
	case GAME_OVER:
		SwapToGameOverBg();
		break;
	case CREDITS:
		SwapToCreditsBg();
		break;
	}

	if (new_state == GAME)
	{
		ResetGameState();
		// Start game music
	}

	ResetSpritesPosition();
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
	SetSprite(16 + current_post_id, post->profile_id);
	MoveMediumSprite(16 + current_post_id, 8 + x_offset, 8 + y_offset);
	// Image
	SetSprite(21 + current_post_id, post->pic_id);
	MoveBigSprite(21 + current_post_id, 32 + x_offset, 8 + y_offset);
	// Text
	DrawText(
		post->first,
		72 + x_offset,
		16 + y_offset + TEXT_Y_OFFSET, FALSE);
	DrawText(
		post->second,
		72 + x_offset,
		24 + y_offset + TEXT_Y_OFFSET, FALSE);
	DrawText(
		post->third,
		72 + x_offset,
		32 + y_offset + TEXT_Y_OFFSET, FALSE);

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

	// Three posts is enough

	// DrawPost(20, 120 + y_offset, &posts[3]);
}

void DrawPostBg(u16 id, s16 pos_x, s16 pos_y)
{
	if (pos_x < -64)
		pos_x = SCREEN_WIDTH;

	sprites[120 + id].attribute0 = COLOR_256 | MOSAIC | WIDE | 8 + pos_y;
	sprites[120 + id].attribute1 = SIZE_64 | 64 + pos_x;
	sprites[120 + id].attribute2 = WHITE_RECTANGLE_ADDR;

	sprites[121 + id].attribute0 = COLOR_256 | MOSAIC | WIDE | 8 + pos_y;
	sprites[121 + id].attribute1 = SIZE_64 | 128 + pos_x;
	sprites[121 + id].attribute2 = WHITE_RECTANGLE_ADDR;
}

void DrawCountdownBar()
{
	for (u16 i = 0; i < 4; i++)
	{
		sprites[116 + i].attribute0 = COLOR_256 | MOSAIC | WIDE | 143;
		sprites[116 + i].attribute1 = SIZE_64 | countdown - (60 * (i + 1)) - 16;
		sprites[116 + i].attribute2 = WHITE_RECTANGLE_ADDR;
	}
}

void ProcessMenuScreen()
{
	if (KeyReleased(KEY_R) && menu_lock == FALSE && mosaic_effect == FALSE)
	{
		StartMosaicEffect();
		next_state = TUTORIAL;
	}
	else if (KeyReleased(KEY_L) && menu_lock == FALSE && mosaic_effect == FALSE)
	{
		StartMosaicEffect();
		next_state = CREDITS;
	}
	else
		menu_lock = FALSE;

	if (MOSAIC_EFFECT_ENDED)
		ChangeState(next_state);
}

void ProcessGameOverScreen()
{
	if (KeyReleased(KEY_R) && menu_lock == FALSE)
		ChangeState(GAME);
	else
		menu_lock = FALSE;
}

void ProcessCreditsScreen()
{
	if (KeyReleased(KEY_R) && menu_lock == FALSE && mosaic_effect == FALSE)
		StartMosaicEffect();
	else
		menu_lock = FALSE;

	if (MOSAIC_EFFECT_ENDED)
		ChangeState(MENU);
}

void EvaluateEndTutorial()
{
	if (tutorial_posts[0].is_last_one == TRUE && mosaic_effect == FALSE)
		StartMosaicEffect();

	if (MOSAIC_EFFECT_ENDED)
		ChangeState(GAME);
}

void DrawMenuScreen()
{
	SetTextColor(COLOR_WHITE);

	// bg
	// falling hearts draw
	// subtitle
	DrawText("(PULISCI L'INTERNET)", CalculateCenterX(20), 80, TRUE);
}

void DrawTutorialPost(s16 x_offset, s16 y_offset, tutorial_post_t *post)
{
	if (post->is_last_one == FALSE)
	{
		// Post bg
		DrawPostBg(current_post_id, x_offset, y_offset);

		sprites[112 + current_post_id].attribute0 = COLOR_256 | MOSAIC | WIDE | 8 + y_offset;
		sprites[112 + current_post_id].attribute1 = SIZE_64 | 32 + x_offset;
		sprites[112 + current_post_id].attribute2 = WHITE_RECTANGLE_ADDR;

		// Profile pic
		SetSprite(16 + current_post_id, 84);
		MoveMediumSprite(16 + current_post_id, 8 + x_offset, 8 + y_offset);

		if (current_post_id < 6)
		{
			// Text
			DrawText(
				post->first,
				36 + x_offset,
				16 + y_offset + TEXT_Y_OFFSET, FALSE);
			DrawText(
				post->second,
				36 + x_offset,
				24 + y_offset + TEXT_Y_OFFSET, FALSE);
			DrawText(
				post->third,
				36 + x_offset,
				32 + y_offset + TEXT_Y_OFFSET, FALSE);
		}
	}
	else
	{
		sprites[112 + current_post_id].attribute0 = COLOR_256 | MOSAIC | WIDE | SCREEN_HEIGHT;
		sprites[112 + current_post_id].attribute1 = SIZE_64 | SCREEN_WIDTH;

		DrawPostBg(current_post_id, SCREEN_WIDTH, SCREEN_HEIGHT);
		MoveMediumSprite(16 + current_post_id, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	current_post_id += 2;
}

void DrawTutorialPosts()
{
	SetTextColor(COLOR_BLACK);

	s16 y_offset = posts_y_offset + 16;
	s16 x_offset = posts_x_offset;

	current_post_id = 0;

	DrawTutorialPost(20 + x_offset, 0 + y_offset, &tutorial_posts[0]);
	DrawTutorialPost(20, 40 + y_offset, &tutorial_posts[1]);
	DrawTutorialPost(20, 80 + y_offset, &tutorial_posts[2]);
	DrawTutorialPost(20, 120 + y_offset, &tutorial_posts[3]);
}

void DrawGameOverScreen()
{
	SetTextColor(COLOR_WHITE);

	char score_text[] = "IL TUO PUNTEGGIO E':    ";
	sprintf(&score_text[21], "%i", score);

	DrawText("GAME OVER", CalculateCenterX(9), 40, TRUE);
	DrawText("SEI STATO CACCIATO!", CalculateCenterX(19), 64, FALSE);
	DrawText(score_text, CalculateCenterX(23), 88, FALSE);
	DrawText("PREMI R PER RIPROVARE", CalculateCenterX(21), 112, FALSE);
}

void DrawCreditsScreen()
{
	SetTextColor(COLOR_WHITE);

	DrawText("MADE WITH <3 BY PIERETTINI", CalculateCenterX(26), 36, TRUE);
}

void SetTextColor(u16 color)
{
	OBJ_PaletteMem[TEXT_PALETTE_ADDR] = color;
}

void SwapToTitleBg()
{
	for (u16 i = 0; i < 256; i++)
		BG_PaletteMem[i] = title_bg_palette[i];

	memset(FrontBuffer, 0x01, SCREEN_WIDTH * SCREEN_HEIGHT);
	// Put at height 38
	memcpy(FrontBuffer + (19 * SCREEN_WIDTH), title_bg_data, sizeof(title_bg_data));

	BG_PaletteMem[TEXT_PALETTE_ADDR] = COLOR_WHITE;
	BG_PaletteMem[0] = COLOR_LIGHT_BLUE;

	DrawTextBg("PREMI R PER INIZIARE", CalculateCenterX(20), 104);
	DrawTextBg("PREMI L PER I CREDITS", CalculateCenterX(21), 120);
}

void SwapToGameBg()
{
	for (u16 i = 0; i < 256; i++)
		BG_PaletteMem[i] = game_bg_palette[i];

	memcpy(FrontBuffer, game_bg_data, sizeof(game_bg_data));
}

void SwapToTutorialBg()
{
	for (u16 i = 0; i < 256; i++)
		BG_PaletteMem[i] = COLOR_DARK_BLUE;

	BG_PaletteMem[TEXT_PALETTE_ADDR] = COLOR_WHITE;

	memset(FrontBuffer, 0x0000, SCREEN_WIDTH * SCREEN_HEIGHT);

	DrawTextBg("PREMI L O R PER CONTINUARE", CalculateCenterX(26), 8);
}

void SwapToGameOverBg()
{
	for (u16 i = 0; i < 256; i++)
		BG_PaletteMem[i] = COLOR_DARK_BLUE;

	memset(FrontBuffer, 0x0000, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void SwapToCreditsBg()
{
	for (u16 i = 0; i < 256; i++)
		BG_PaletteMem[i] = COLOR_LIGHT_BLUE;

	BG_PaletteMem[TEXT_PALETTE_ADDR] = COLOR_WHITE;

	memset(FrontBuffer, 0x0000, SCREEN_WIDTH * SCREEN_HEIGHT);

	DrawTextBg("ART: PIERA FALCONE", CalculateCenterX(18), 60);
	DrawTextBg("CODE: GIORGIO POMETTINI", CalculateCenterX(23), 76);
	DrawTextBg("MUSIC: TECLA ZORZI", CalculateCenterX(18), 92);

	DrawTextBg("PREMI R PER USCIRE", CalculateCenterX(18), 116);
}

inline u16 CalculateCenterX(u16 char_num)
{
	return (SCREEN_WIDTH / 2) - ((char_num * 8) / 2);
}

void StartMosaicEffect()
{
	mosaic_delta = 1;
	mosaic_amount = 1;
	mosaic_effect = TRUE;
}

void ProcessMosaicEffect()
{
	if (mosaic_effect == TRUE)
	{
		mosaic_amount += mosaic_delta;

		if (mosaic_amount >= MOSAIC_MAX)
			mosaic_delta = -1;

		if (mosaic_amount == MOSAIC_MIN)
			mosaic_effect = FALSE;
	}

	REG_MOSAIC = mosaic_amount * 0x1111;
}

int rand()
{
	static int work = 0xD371F947;
	work = work * 0x41C64E6D + 0x3039;
	return ((work >> 16) & 0x7FFF);
}

void PlotPixel(u16 x, u16 y, u8 color)
{
	// Stolen from TONC lib
	u16 *destination = &FrontBuffer[(y * SCREEN_WIDTH + x) >> 1];
	if (x & 1)
		*destination = (*destination & 0xFF) | (color << 8);
	else
		*destination = (*destination & ~0xFF) | color;
}

void PlayMusic()
{
	REG_SGCNT0_H = DSOUND_A_RIGHT_CHANNEL | DSOUND_A_LEFT_CHANNEL | DSOUND_A_FIFO_RESET;
	REG_SGCNT1 = SOUND_MASTER_ENABLE;
	REG_DM1SAD = (u32)africa;
	REG_DM1DAD = 0x40000A0;
	REG_DM1CNT_H = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 | DMA_TIMING_SYNC_TO_DISPLAY | DMA_ENABLE;

	// Sample rate
	REG_TM0D = 65536 - (16777216 / 11025);
	REG_TM0CNT = TIMER_ENABLE;
}

void _Init()
{
	SetMode(MODE_4 | BG2_ENABLE | OBJ_ENABLE | OBJ_MAP_2D);

	// Enable mosaic on Background 2
	REG_BG2CNT |= BG_MOSAIC_ENABLE;

	// Copy palette data
	for (u16 i = 0; i < 256; i++)
		OBJ_PaletteMem[i] = tileset_palette[i];

	// Copy sprite data
	memcpy(OAM_Data, tileset_data, sizeof(tileset_data));

	// Copy font data (font palette is 0xFF)
	memcpy(OAM_Data + 4096 + 1024, font_data, sizeof(font_data));

	// Put last part of spritesheet to white color
	OBJ_PaletteMem[POST_BG_PALETTE_ADDR] = COLOR_WHITE;
	memset(OAM_Data + 4096, POST_BG_PALETTE_ADDR, 4096);

	SwapToTitleBg();

	InitializeSprites();
	GeneratePosts();
	InsertTutorialPosts();
	ResetSpritesPosition();
}

void _Update()
{
	if (game_state == MENU)
	{
		ProcessMenuScreen();
	}
	else if (game_state == TUTORIAL)
	{
		ProcessButtons();
		ProcessFeedAnimation();
		ProcessSwipeAnimation();
		EvaluateEndTutorial();
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
		ProcessGameOverScreen();
	}
	else if (game_state == CREDITS)
	{
		ProcessCreditsScreen();
	}
}

void _Draw()
{
	if (game_state == MENU)
	{
		DrawMenuScreen();
	}
	else if (game_state == TUTORIAL)
	{
		DrawTutorialPosts();
	}
	else if (game_state == GAME)
	{
		DrawPosts();
		DrawCountdownBar();
	}
	else if (game_state == GAME_OVER)
	{
		DrawGameOverScreen();
	}
	else if (game_state == CREDITS)
	{
		DrawCreditsScreen();
	}

	ProcessMosaicEffect();
	CleanCharacterSprites();
}

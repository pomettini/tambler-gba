#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gba.h"
#include "sprites.h"
#include "font.h"
#include "background.h"
#include "game_bg.h"
#include "title_bg.h"
#include "main.h"

// TODO: Remove all magic numbers
// TODO: Add sprite flipping

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
const u16 countdown_speed = 3;
const u16 text_y_offset = -4;

// GBA stuff
OAMEntry sprites[SPRITE_NUM];

u16 pos_x = 0;
u16 pos_y = 0;
u16 palette = 0;

u16 current_char = 0;
u16 current_post_id = 0;

u16 __key_curr = 0, __key_prev = 0;

int main()
{
	_Init();

	for (ever)
	{
		current_char = 0;

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
	for (int x = 0; x < SPRITE_NUM; x++)
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
		sprites[i].attribute0 = COLOR_256 | SQUARE | i * 8;
		sprites[i].attribute1 = SIZE_8 | i * 8;
		sprites[i].attribute2 = 2 + (i * 2);
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
	sprites[BIG_SPR_START_OFFSET + BIG_SPR_NUM + current_char].attribute2 = 512 + 256 + 64 + ((int)char_ * 2);

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

void SetSprite(u16 oam_id, u16 sprite_id)
{
	sprites[oam_id].attribute2 = 512 + sprite_id;
}

void KeyPoll()
{
	__key_prev = __key_curr;
	__key_curr = ~KEYS & 0x03FF;
}

u32 KeyReleased(u32 key)
{
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

	// TODO: Refactor this
	tutorial_posts[0] = tutorial_posts[1];
	tutorial_posts[1] = tutorial_posts[2];
	tutorial_posts[2] = tutorial_posts[3];
	tutorial_posts[3] = tutorial_posts[4];
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
		countdown -= countdown_speed;
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
		SwapToDarkBlueBg();
		break;
	case GAME:
		SwapToGameBg();
		SetTextColor(COLOR_BLACK);
		break;
	case GAME_OVER:
		SwapToDarkBlueBg();
		break;
	case CREDITS:
		SwapToLightBlueBg();
		break;
	}

	if (new_state == GAME)
	{
		ResetGameState();
		StartGameMusic();
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

	// Three posts is enough

	// DrawPost(20, 120 + y_offset, &posts[3]);
}

void DrawPostBg(u16 id, s16 pos_x, s16 pos_y)
{
	if (pos_x < -64)
		pos_x = 256;

	sprites[120 + id].attribute0 = COLOR_256 | WIDE | 8 + pos_y;
	sprites[120 + id].attribute1 = SIZE_64 | 64 + pos_x;
	sprites[120 + id].attribute2 = 768;

	sprites[121 + id].attribute0 = COLOR_256 | WIDE | 8 + pos_y;
	sprites[121 + id].attribute1 = SIZE_64 | 128 + pos_x;
	sprites[121 + id].attribute2 = 768;
}

void DrawCountdownBar()
{
	// TODO: Refactor this

	sprites[116].attribute0 = COLOR_256 | WIDE | 143;
	sprites[116].attribute1 = SIZE_64 | countdown - 60 - 16;
	sprites[116].attribute2 = 768;

	sprites[117].attribute0 = COLOR_256 | WIDE | 143;
	sprites[117].attribute1 = SIZE_64 | countdown - 120 - 16;
	sprites[117].attribute2 = 768;

	sprites[118].attribute0 = COLOR_256 | WIDE | 143;
	sprites[118].attribute1 = SIZE_64 | countdown - 180 - 16;
	sprites[118].attribute2 = 768;

	sprites[119].attribute0 = COLOR_256 | WIDE | 143;
	sprites[119].attribute1 = SIZE_64 | countdown - 240 - 16;
	sprites[119].attribute2 = 768;

	// char text[12];
	// sprintf(text, "%i", score);

	// DrawText(text, 0, 0);
}

void ProcessMenuScreen()
{
	if (KeyReleased(KEY_R) && menu_lock == FALSE)
		ChangeState(TUTORIAL);
	else if (KeyReleased(KEY_L) && menu_lock == FALSE)
		ChangeState(CREDITS);
	else
		menu_lock = FALSE;
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
	if (KeyReleased(KEY_R) && menu_lock == FALSE)
		ChangeState(MENU);
	else
		menu_lock = FALSE;
}

void EvaluateEndTutorial()
{
	if (tutorial_posts[0].is_last_one == TRUE)
		ChangeState(GAME);
}

void DrawMenuScreen()
{
	SetTextColor(COLOR_WHITE);

	// bg
	// falling hearts draw
	// subtitle
	DrawText("(PULISCI L'INTERNET)", CalculateCenterX(20), 80);

	DrawText("PREMI R PER INIZIARE", CalculateCenterX(20), 104);
	DrawText("PREMI L PER I CREDITS", CalculateCenterX(21), 120);
}

void DrawTutorialPost(s16 x_offset, s16 y_offset, tutorial_post_t *post)
{
	if (post->is_last_one == FALSE)
	{
		// Post bg
		DrawPostBg(current_post_id, x_offset, y_offset);

		sprites[112 + current_post_id].attribute0 = COLOR_256 | WIDE | 8 + y_offset;
		sprites[112 + current_post_id].attribute1 = SIZE_64 | 32 + x_offset;
		sprites[112 + current_post_id].attribute2 = 768;

		// Profile pic
		SetSprite(16 + current_post_id, 84);
		MoveMediumSprite(16 + current_post_id, 8 + x_offset, 8 + y_offset);

		if (current_post_id < 6)
		{
			// Text
			DrawText(
				post->first,
				36 + x_offset,
				16 + y_offset + text_y_offset);
			DrawText(
				post->second,
				36 + x_offset,
				24 + y_offset + text_y_offset);
			DrawText(
				post->third,
				36 + x_offset,
				32 + y_offset + text_y_offset);
		}
	}
	else
	{
		sprites[112 + current_post_id].attribute0 = COLOR_256 | WIDE | 160;
		sprites[112 + current_post_id].attribute1 = SIZE_64 | 240;

		DrawPostBg(current_post_id, 240, 160);
		MoveMediumSprite(16 + current_post_id, 240, 160);
	}

	current_post_id += 2;
}

void DrawTutorialPosts()
{
	SetTextColor(COLOR_BLACK);

	// DrawText("PREMI R", CalculateCenterX(9), 8);

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

	DrawText("GAME OVER", CalculateCenterX(9), 40);
	DrawText("SEI STATO CACCIATO!", CalculateCenterX(19), 64);
	DrawText(score_text, CalculateCenterX(23), 88);
	DrawText("PREMI R PER RIPROVARE", CalculateCenterX(21), 112);
}

void DrawCreditsScreen()
{
	SetTextColor(COLOR_WHITE);

	DrawText("TAMBLER", CalculateCenterX(7), 40);
	DrawText("A PIERETTINI GAME", CalculateCenterX(17), 56);

	DrawText("ART: PIERA FALCONE", CalculateCenterX(18), 80);
	DrawText("CODE: GIORGIO POMETTINI", CalculateCenterX(23), 96);
	// DrawText("MUSIC: TECLA ZORZI", 0, 32); <- Sorry Tecla :(

	DrawText("PREMI R PER USCIRE", CalculateCenterX(18), 120);
}

void SetTextColor(u16 color)
{
	OBJ_PaletteMem[TEXT_PALETTE_ADDR] = color;
}

void SwapToTitleBg()
{
	for (u16 i = 0; i < 256; i++)
		BG_PaletteMem[i] = title_bg_palette[i];

	memset(FrontBuffer, 0x01, 240 * 160);
	// Put at height 38
	memcpy(FrontBuffer + (19 * 240), title_bg_data, sizeof(title_bg_data));
}

void SwapToGameBg()
{
	for (u16 i = 0; i < 256; i++)
		BG_PaletteMem[i] = game_bg_palette[i];

	memcpy(FrontBuffer, game_bg_data, sizeof(game_bg_data));
}

void SwapToDarkBlueBg()
{
	for (u16 i = 0; i < 256; i++)
		BG_PaletteMem[i] = COLOR_DARK_BLUE;

	memset(FrontBuffer, 0x0000, 240 * 160);
}

void SwapToLightBlueBg()
{
	for (u16 i = 0; i < 256; i++)
		BG_PaletteMem[i] = COLOR_LIGHT_BLUE;

	memset(FrontBuffer, 0x0000, 240 * 160);
}

inline u16 CalculateCenterX(u16 char_num)
{
	return (SCREEN_WIDTH / 2) - ((char_num * 8) / 2);
}

void StartGameMusic()
{
}

void _Init()
{
	SetMode(MODE_4 | BG2_ENABLE | OBJ_ENABLE | OBJ_MAP_2D);

	// TODO: Find a way to generate a random seed
	srand(10);

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

	CleanCharacterSprites();
}

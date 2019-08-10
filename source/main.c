#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gba.h"
#include "sprites.h"
#include "font.h"
#include "game_background.h"
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

#define REG_BG0CNT *(volatile u16 *)0x4000008

int main()
{
	SetMode(MODE_0 | BG0_ENABLE | OBJ_ENABLE | OBJ_MAP_2D);

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
	{
		OAM_Mem[loop] = temp[loop];
	}
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
	sprites[oam_id].attribute2 = sprite_id;
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

void LoadTutorialPosts()
{
}

void GeneratePosts()
{
	for (u16 i = 0; i < POST_NUM; i++)
		posts[i] = GeneratePost();
}

void InsertTutorialPosts()
{
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

	sprites[120 + id].attribute0 = COLOR_256 | WIDE | ROTATION_FLAG | SIZE_DOUBLE | 8 + pos_y;
	sprites[120 + id].attribute1 = SIZE_32 | ROTDATA(0) | 64 + pos_x;
	sprites[120 + id].attribute2 = 256;

	sprites[121 + id].attribute0 = COLOR_256 | WIDE | ROTATION_FLAG | SIZE_DOUBLE | 8 + pos_y;
	sprites[121 + id].attribute1 = SIZE_32 | ROTDATA(0) | 128 + pos_x;
	sprites[121 + id].attribute2 = 256;
}

void DrawCountdownBar()
{
	sprites[118].attribute0 = COLOR_256 | WIDE | ROTATION_FLAG | SIZE_DOUBLE | 142;
	sprites[118].attribute1 = SIZE_64 | ROTDATA(0) | countdown - 120 - 16;
	sprites[118].attribute2 = 256;

	sprites[119].attribute0 = COLOR_256 | WIDE | ROTATION_FLAG | SIZE_DOUBLE | 142;
	sprites[119].attribute1 = SIZE_64 | ROTDATA(0) | countdown - 240 - 16;
	sprites[119].attribute2 = 256;

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
}

void DrawMenuScreen()
{
	SetTextColor(COLOR_WHITE);

	// bg
	// falling hearts draw
	// title screen
	DrawText("TAMBLER", 0, 0);
	// subtitle
	DrawText("(PULISCI L'INTERNET)", 0, 16);

	DrawText("PREMI R PER INIZIARE", 0, 32);
	DrawText("PREMI L PER I CREDITS", 0, 40);
}

void DrawTutorialPosts()
{
	SetTextColor(COLOR_WHITE);

	DrawText("(TUTORIAL)", 0, 0);
	DrawText("PREMI R PER AVANZARE", 0, 16);
}

void DrawGameOverScreen()
{
	SetTextColor(COLOR_WHITE);

	DrawText("GAME OVER", 0, 0);
	DrawText("SEI STATO CACCIATO!", 0, 16);
	DrawText("IL TUO PUNTEGGIO E':", 0, 32);

	char score_text[3];
	sprintf(score_text, "%i", score);
	DrawText(score_text, 168, 32);

	DrawText("PREMI R PER RIPROVARE", 0, 48);
}

void DrawCreditsScreen()
{
	SetTextColor(COLOR_WHITE);

	DrawText("TAMBLER THE GAME", 0, 0);
	DrawText("A PIERETTINI PRODUCTION", 0, 16);
	DrawText("ART: P. FALCONE", 0, 32);
	DrawText("CODE: G. POMETTINI", 0, 40);
	// DrawText("MUSIC: T. ZORZI", 0, 32);
	DrawText("PREMI R PER USCIRE", 0, 56);
}

void SetTextColor(u16 color)
{
	OBJ_PaletteMem[TEXT_PALETTE_ADDR] = color;
}

void StartGameMusic()
{
}

void _Init()
{
	// TODO: Find a way to generate a random seed
	srand(10);

	REG_BG0CNT = 0x1F83;

	// Copy palette data
	for (u16 i = 0; i < 256; i++)
	{
		OBJ_PaletteMem[i] = tileset_palette[i];
		BG_PaletteMem[i] = game_background_palette[i];
	}

	// Copy sprite and bg data
	memcpy(FrontBuffer, game_background_data, sizeof(game_background_data));
	memcpy(OAM_Data, tileset_data, sizeof(tileset_data));

	// Put last part of spritesheet to white color
	OBJ_PaletteMem[POST_BG_PALETTE_ADDR] = COLOR_WHITE;
	memset(OAM_Data + 4096, POST_BG_PALETTE_ADDR, 4096);

	// Was 4096
	// Copy font data (font palette is 0xFF)
	memcpy(OAM_Data + 8192, font_data, sizeof(font_data));

	InitializeSprites();

	ResetSpritesPosition();

	GeneratePosts();
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

		if (KeyReleased(KEY_R))
			ChangeState(GAME);
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
		SetTextColor(COLOR_BLACK);
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

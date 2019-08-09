#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gba.h"
#include "sprites.h"
#include "font.h"
#include "game_background.h"
#include "main.h"

// TODO: Remove all magic numbers

// States
u16 game_state = GAME;

// Gameplay globals
post_t posts[3];
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

	_Init();

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
		sprites[x].attribute0 = 160;
		sprites[x].attribute1 = 240;
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
		sprites[i].attribute0 = 160;
		sprites[i].attribute1 = 240;
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

void LoadTutorialPosts()
{
}

void GeneratePosts()
{
	for (u16 i = 0; i < 3; i++)
		posts[i] = GeneratePost();
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
		PostAnimationEnded();
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
	{
		PopAndPushTutorial();
	}

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

void EvaluateEndTutorial()
{
}

void ProcessMenuScreen()
{
}

void ProcessGameOverScreen()
{
}

void ProcessCreditsScreen()
{
}

void DrawMenuScreen()
{
}

void DrawTutorialPosts()
{
}

void DrawGameOverScreen()
{
	DrawText("GAME OVER", 0, 0);
	DrawText("SEI STATO CACCIATO!", 0, 8);
	DrawText("IL TUO PUNTEGGIO E':", 0, 16);
	DrawText("PREMI R PER RIPROVARE", 0, 24);
}

void DrawCreditsScreen()
{
	DrawText("TAMBLER THE GAME", 0, 0);
	DrawText("A PIERETTINI PRODUCTION", 0, 8);
	DrawText("ART: PIERA FALCONE", 0, 16);
	DrawText("CODE: GIORGIO POMETTINI", 0, 24);
	DrawText("MUSIC: TECLA ZORZI", 0, 32);
	DrawText("PREMI R PER USCIRE", 0, 40);
}

void StartGameMusic()
{
}

void _Init()
{
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

	memset(OAM_Data + 4096, COLOR_WHITE, 4096);
	// Was 4096
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

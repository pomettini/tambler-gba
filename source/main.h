#include "gba.h"

#define FALSE 0
#define TRUE 1

#define ever \
    ;        \
    ;

#define SMALL_SPR_NUM 16
#define MEDIUM_SPR_NUM 8
#define BIG_SPR_NUM 4

#define SMALL_SPR_START_OFFSET 0
#define MEDIUM_SPR_START_OFFSET 16
#define BIG_SPR_START_OFFSET 24

#define COUNTDOWN_START_VAL 100

enum states
{
    MENU = 0,
    TUTORIAL = 1,
    GAME = 2,
    GAME_OVER = 3,
    CREDITS = 4
};

typedef struct database
{
    char text[3][11];
    u16 is_valid;
} database_t;

typedef struct post
{
    char text[3][11];
    u16 is_valid;
    u16 profile_id;
    u16 pic_id;
} post_t;

void CopyOAM();
void ResetSpritesPosition();
void InitializeSprites();
void MoveSmallSprite(u16 id, u16 pos_x, u16 pos_y);
void MoveMediumSprite(u16 id, u16 pos_x, u16 pos_y);
void MoveBigSprite(u16 id, u16 pos_x, u16 pos_y);

void LoadDatabase();
void LoadTutorialPost();
void GeneratePosts();
void PopAndPushPost();
void PopAndPushTutorial();
void ProcessButtons();
void ProcessFeedAnimation();
void ProcessSwipeAnimation();
void DecreaseCountdown();
void PostAnimationEnded();
void EvaluateContent(post_t *post);
void EvaluateGameOver();
void ChangeState(u16 new_state);
void ResetGameState();
void StartGameMusic();

void _Init();
void _Update();
void _Draw();
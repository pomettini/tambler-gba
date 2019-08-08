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
#define POSTS_NUM 4
#define TEXT_LEN_MAX 13

#define LETTERS_NUM 26

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
    char first[TEXT_LEN_MAX];
    char second[TEXT_LEN_MAX];
    char third[TEXT_LEN_MAX];
    u16 is_valid;
} database_t;

typedef struct post
{
    char first[TEXT_LEN_MAX];
    char second[TEXT_LEN_MAX];
    char third[TEXT_LEN_MAX];
    u16 is_valid;
    u16 profile_id;
    u16 pic_id;
} post_t;

void CopyOAM();
void ResetSpritesPosition();
void InitializeSprites();
void DrawCharacter(char char_, u16 pos_x, u16 pos_y);
void MoveSmallSprite(u16 id, s16 pos_x, s16 pos_y);
void MoveMediumSprite(u16 id, s16 pos_x, s16 pos_y);
void MoveBigSprite(u16 id, s16 pos_x, s16 pos_y);

void LoadTutorialPosts();
void GeneratePosts();
void PopAndPushPost();
void PopAndPushTutorial();
post_t GetRandomPost();
u16 GetRandomProfilePic();
u16 GetRandomPostPic();
void ProcessButtons();
void ProcessFeedAnimation();
void ProcessSwipeAnimation();
void DecreaseCountdown();
void PostAnimationEnded();
void EvaluateContent(post_t *post);
void EvaluateGameOver();
void ChangeState(u16 new_state);
void ResetGameState();
void DrawPost(s16 x_offset, s16 y_offset, post_t *post);
void DrawPosts();
void StartGameMusic();

void _Init();
void _Update();
void _Draw();

const post_t DATABASE[] = {
    {"DONNA CALDA", "A 2 KM", "DA TE", FALSE},
    {"VENDESI", "SOPRAMMOBILI", "USATI", TRUE},
    {"CLICCA QUI", "PER IL", "CA**O", FALSE},
    {"PASSATA DI", "POMODORO", "TRE PER DUE", TRUE}};

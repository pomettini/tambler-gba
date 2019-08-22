#include "gba.h"

#define FALSE 0
#define TRUE 1

// For the lulz

#define ever \
    ;        \
    ;

#define SPRITE_NUM 128
#define POST_NUM 3
#define TUTORIAL_POST_NUM 5

#define SMALL_SPR_NUM 16
#define MEDIUM_SPR_NUM 8
#define BIG_SPR_NUM 4

#define SMALL_SPR_START_OFFSET 0
#define MEDIUM_SPR_START_OFFSET 16
#define BIG_SPR_START_OFFSET 24
#define TEXT_SPR_OFFSET 28

#define POSTS_NUM 34

#define COUNTDOWN_START_VAL 240
#define COUNTDOWN_SECOND_START_VAL 20
#define TEXT_LEN_MAX 13
#define TUTORIAL_TEXT_LEN_MAX 19
#define LETTERS_MAX 84 // Was 92

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_LIGHT_BLUE 0x7EA5
#define COLOR_DARK_BLUE 0x28A3

#define POST_BG_PALETTE_ADDR 0xFE
#define TEXT_PALETTE_ADDR 0xFF

#define SPRITE_START_ADDR 512
#define WHITE_RECTANGLE_ADDR 768

#define MOSAIC_MIN 0
#define MOSAIC_MAX 14

// Constant functions

#define MOSAIC_EFFECT_ENDED mosaic_amount == MOSAIC_MAX
#define CALCULATE_CENTER_X(char_num) (SCREEN_WIDTH / 2) - ((char_num * 8) / 2)
#define SIN_LUT_MAX 42
const s16 SIN_LUT[] = {0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 0, 0, -1, -1, -1, -2, -2, -2, -2, -3, -3, -3, -3, -3, -2, -2, -2, -2, -1, -1, -1};

// Registers

#define REG_SGCNT0_H *(u16 *)0x4000082
#define REG_SGCNT1 *(u16 *)0x4000084
#define REG_DM1SAD *(u32 *)0x40000BC
#define REG_DM1DAD *(u32 *)0x40000C0
#define REG_DM1CNT_H *(u16 *)0x40000C6
#define REG_TM0D *(u16 *)0x4000100
#define REG_TM0CNT *(u16 *)0x4000102

#define DSOUND_A_RIGHT_CHANNEL 256
#define DSOUND_A_LEFT_CHANNEL 512
#define DSOUND_A_FIFO_RESET 2048
#define SOUND_MASTER_ENABLE 128

#define DMA_DEST_FIXED 64
#define DMA_REPEAT 512
#define DMA_32 1024
#define DMA_TIMING_SYNC_TO_DISPLAY 4096 | 8192
#define DMA_ENABLE 32768
#define TIMER_ENABLE 128

#define REG_BG2CNT *(u16 *)0x400000C
#define REG_MOSAIC *(u32 *)0x400004C

// Structures

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

typedef struct tutorial_post
{
    char first[TUTORIAL_TEXT_LEN_MAX];
    char second[TUTORIAL_TEXT_LEN_MAX];
    char third[TUTORIAL_TEXT_LEN_MAX];
    u16 is_last_one;
} tutorial_post_t;

// ROM data

const post_t DATABASE[] = {
    {"DONNA CALDA", "A 2 KM", "DA TE", FALSE},
    {"VENDESI", "SOPRAMMOBILI", "USATI", TRUE},
    {"CLICCA QUI", "PER IL", "CA**O", FALSE},
    {"PASSATA DI", "POMODORO", "TRE PER DUE", TRUE},
    {"VENDO", "LA DROGA", "", FALSE},
    {"QUI FOTO", "DI GATTINI", "", TRUE},
    {"QUI FOTO", "DI", "CAGNOLINI", TRUE},
    {"BISCOTTI", "IN OFFERTA", "", TRUE},
    {"SITO", "BELLISSIMO", "SUI MEMINI", TRUE},
    {"GUARDATE CHE", "BELLE LE", "MIE TETTE", FALSE},
    {"NUOVISSIMI", "GIOCHI PER", "PLAYSTATION", TRUE},
    {"ALTRI", "MEMINI", "DIVERTENTI", TRUE},
    {"QUI BATTUTE", "SUI CODER", "", TRUE},
    {"LEGGI", "HENTAI", "ONLINE", FALSE},
    {"NUOVO MODO", "ALLUNGAMENTO", "DEL PENE", FALSE},
    {"VISITE", "GRATUITE", "UROLOGO", TRUE},
    {"NUOVA DIETA", "DI KYLIE", "JENNER", TRUE},
    {"QUI KIM", "KARDASHIAN", "NUDA", FALSE},
    {"JASON MOMOA", "LEAKED", "PHOTO", FALSE},
    {"10 MOTIVI", "PER SMETTERE", "DI FUMARE", TRUE},
    {"LE MIGLIORI", "10 METE", "EUROPEE", TRUE},
    {"MOBILI", "IKEA IN", "IPER OFFERTA", TRUE},
    {"UOMO CALDO", "A 3 KM", "DA TE", FALSE},
    {"COPPIA DI", "SCAMBISTI", "CERCA DONNA", FALSE},
    {"SITO PER", "ARTICOLI", "DA PESCA", TRUE},
    {"SITO PER", "OGGETTI", "CINESISSIMI", TRUE},
    {"FAMMI VEDERE", "I TUOI", "PIEDINI", FALSE},
    {"NUOVE", "RICETTINE SU", "VRDBASILICO", TRUE},
    {"PREVENDITA", "CONCERTO DI", "GG D'ALESSIO", TRUE},
    {"VENDITA", "ACCESSORI DI", "HELLO KITTY", TRUE},
    {"VIDEO OSE'", "PER", "OMOSESSUALI", FALSE},
    {"FOTO DI", "PENI", "EXTRA-LARGE", FALSE},
    {"RIHANNA", "LEAKED", "PHOTO", FALSE},
    {"SITO DI", "BARZELLETTE", "DIVERTENTI", TRUE},
};

const tutorial_post_t TUTORIAL_POSTS[] = {
    {"SEI ROB, IL", "NUOVO MOD.", "DI TAMBLER", FALSE},
    {"DEVI ELIMINARE I", "POST ESPLICITI", "", FALSE},
    {"PREMI R PER", "APPROVARE", "", FALSE},
    {"PREMI L PER", "ELIMINARE", "", FALSE},
    {"SE SBAGLI SARAI", "LICENZIATO!!!", "", FALSE},
};

// Forward declarations

void CopyOAM();
void ResetSpritesPosition();
void InitializeSprites();
void DrawCharacter(unsigned char char_, s16 pos_x, s16 pos_y, u16 wave);
void DrawText(unsigned char *text, s16 pos_x, s16 pos_y, u16 wave);
void DrawCharacterBg(unsigned char char_, s16 pos_x, s16 pos_y);
void DrawTextBg(unsigned char *text, s16 pos_x, s16 pos_y);
void CleanCharacterSprites();
void MoveSmallSprite(u16 id, s16 pos_x, s16 pos_y);
void MoveMediumSprite(u16 id, s16 pos_x, s16 pos_y);
void MoveBigSprite(u16 id, s16 pos_x, s16 pos_y);
void SetSprite(u16 oam_id, u16 sprite_id);
void KeyPoll();
u32 KeyReleased(u32 key);

void LoadTutorialPosts();
void GeneratePosts();
post_t GeneratePost();
void PopAndPushPost();
void PopAndPushTutorial();
u16 GetRandomPostId();
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
void DrawPostBg(u16 id, s16 pos_x, s16 pos_y);
void DrawCountdownBar();
void SetTextColor(u16 color);
void StartMosaicEffect();
void ProcessMosaicEffect();
void PlotPixel(u16 x, u16 y, u8 color);
void PlayMusic();

void ProcessMenuScreen();
void ProcessGameOverScreen();
void ProcessCreditsScreen();
void EvaluateEndTutorial();

void DrawMenuScreen();
void DrawTutorialPost(s16 x_offset, s16 y_offset, tutorial_post_t *post);
void DrawTutorialPosts();
void DrawGameOverScreen();
void DrawCreditsScreen();

void SwapToTitleBg();
void SwapToTutorialBg();
void SwapToGameOverBg();
void SwapToGameBg();
void SwapToCreditsBg();

void _Init();
void _Update();
void _Draw();

// Overridden functions

int rand();

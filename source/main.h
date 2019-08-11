#include "gba.h"

#define FALSE 0
#define TRUE 1

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
#define LETTERS_NUM 26
#define LETTERS_MAX 88 // Was 92

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF

#define POST_BG_PALETTE_ADDR 0xFE
#define TEXT_PALETTE_ADDR 0xFF

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
    {"SEI ROB, IL", "NUOVO MODERATORE", "DI TAMBLER", FALSE},
    {"IL TUO LAVORO E'", "ELIMINARE I", "CONTENUTI ESPLICITI", FALSE},
    {"PREMI R PER", "APPROVARE IL POST", "", FALSE},
    {"PREMI L PER", "ELIMINARE IL POST", "", FALSE},
    {"SE SBAGLI SARAI", "LICENZIATO!!!", "", FALSE},
};

void CopyOAM();
void ResetSpritesPosition();
void InitializeSprites();
void DrawCharacter(unsigned char char_, s16 pos_x, s16 pos_y);
void DrawText(unsigned char *text, s16 pos_x, s16 pos_y);
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
void StartGameMusic();

void ProcessMenuScreen();
void ProcessGameOverScreen();
void ProcessCreditsScreen();
void EvaluateEndTutorial();

void DrawMenuScreen();
void DrawTutorialPosts();
void DrawGameOverScreen();
void DrawCreditsScreen();

void _Init();
void _Update();
void _Draw();

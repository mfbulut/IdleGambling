#if defined(PLATFORM_WEB) || defined(_WIN32) || defined(__linux__)
#include "raylib.h"
void Vibrate(float sec) {}
#else
#include "raymob.h"
#endif

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include "raymath.h"

const int screenWidth = 1200;
const int screenHeight = 540;

typedef enum GameScreen { TITLE, SETTINGS, SHOP, GAMEPLAY } GameScreen;

Font font120;
Font font96;
Font font80;
Font font64;
Font font48;
Font font44;
Font font36;
Font font33;
Font font30;
Font font26;
Font font24;
Font font18;
Font font16;

char* text[] = {
     "Play", "Oyna",
     "Settings", "Ayarlar",
     "Shop", "Market",
     "Volume", "Ses Düzeyi",
     "Vibration", "Titreşim",
     "Return", "Geri Dön",
     "Ball", "Misket",
     "Speed", "Hız",
     "Multiplier", "Katlayıcı",
     "Sold Out", "Hepsi Satıldı",
     "Hit", "Kart Çek",
     "Stand", "Kal",
     "Bet: %s", "Bahis: %s",
     "Dealer", "Dağıtıcı",
     "Swap", "Değiştir",
     "Play Again", "Tekrar Oyna",
     "Level Up", "Seviye Atla",
     "Insurance", "Sigorta",
     "Get %10 payback on lose", "Kaybın %10'u geri alınır",
     "Fast Hand", "Hızlı Eller",
     "Swap hands with dealer", "Dağıtıcıyla elini değiştir",
     "The Cheater", "Hilekar",
     "See dealers hand", "Dağıtıcının kartlarını gör",
     "Cancel", "İptal",
     "WIN", "KAZANDIN",
     "KAYBETTİN", "LOSE",
     "DRAW", "BERABERE",
     "Golden Ball", "Altın Top",
     "One ball gives 2X money", "Bir top 2X para kazandırır",
     "Continue", "Devam et",
     "Bonus", "Bonus",
     "Equip", "Seç",
     "Equipped", "Seçili",
};

#define TEXT_PLAY        text[save.lang + 0]
#define TEXT_SETTINGS    text[save.lang + 2]
#define TEXT_SHOP        text[save.lang + 4]
#define TEXT_VOLUME      text[save.lang + 6]
#define TEXT_VIBRATION   text[save.lang + 8]
#define TEXT_RETURN      text[save.lang + 10]
#define TEXT_BALL        text[save.lang + 12]
#define TEXT_SPEED       text[save.lang + 14]
#define TEXT_MULTIPLIER  text[save.lang + 16]
#define TEXT_SOLDOUT     text[save.lang + 18]
#define TEXT_HIT         text[save.lang + 20]
#define TEXT_STAND       text[save.lang + 22]
#define TEXT_BET         text[save.lang + 24]
#define TEXT_DEALER      text[save.lang + 26]
#define TEXT_SWAP_BTN    text[save.lang + 28]
#define TEXT_AGAIN       text[save.lang + 30]
#define TEXT_LEVELUP     text[save.lang + 32]

#define TEXT_RETAINLOSS       text[save.lang + 34]
#define TEXT_RETAINLOSS_DESC  text[save.lang + 36]
#define TEXT_SWAP             text[save.lang + 38]
#define TEXT_SWAP_DESC        text[save.lang + 40]
#define TEXT_SEEFIRST         text[save.lang + 42]
#define TEXT_SEEFIRST_DESC    text[save.lang + 44]

#define TEXT_CANCEL   text[save.lang + 46]
#define TEXT_WIN      text[save.lang + 48]
#define TEXT_LOSE     text[save.lang + 50]
#define TEXT_DRAW     text[save.lang + 52]

#define TEXT_GOLDEN          text[save.lang + 54]
#define TEXT_GOLDEN_DESC     text[save.lang + 56]

#define TEXT_CONTINUE        text[save.lang + 58]
#define TEXT_BONUS           text[save.lang + 60]
#define TEXT_EQUIP           text[save.lang + 62]
#define TEXT_EQUIPPED        text[save.lang + 64]

#define BUTTON_COLOR (Color){ 60, 80, 180, 255 }

typedef struct {
    float volume;
    bool vibration;
    int lang;
    long long money;
    int ballCount;
    int speed;
    int moneyMult;
    int level;
    bool seeFirstCard;
    bool swapHands;
    bool retainLoss;
    bool goldBall;
    int ownedCosmetics;
    int currentCosmetic;
} GameSave;

GameSave save = {
    0.5f,  // volume
    true,  // vibration
    0,     // lang
    0,     // money
    1,     // ballCount
    0,     // speed
    1,     // moneyMult
    0,     // level
    false, // seeFirstCard
    false, // swapHands
    false, // retainLoss
    false, // goldBall
    1,
    0,
};

GameScreen currentScreen = TITLE;

#define MAX_COSMETICS 5

typedef struct {
    char* nameTR;
    char* nameEN;
    int price;
    int bitIndex;
} Cosmetic;

Cosmetic cosmetics[MAX_COSMETICS] = {
    {"Classic", "Klasik", 0, 0},
    {"Modern", "Modern", 1000, 1},
    {"Eagle Pattern", "Kartal Desenli", 30000, 2},
    {"Bull Pattern", "Boğa Desenli",  500000, 4},
    {"Lion Pattern", "Aslan Desenli", 1000000, 4},
};

Texture tables[MAX_COSMETICS];

Texture deckTexture;
Texture chipTexture;
Texture backTexture;
Texture lockTexture;
Texture slotMachineTexture;
Texture slotSpriteSheet;
Texture blueButtonTexture;
Texture redButtonTexture;
Texture greenButtonTexture;
Texture trFlag;
Texture enFlag;
Texture logoTexture;
Texture backgroundPattern;

Sound take4Effect;
Sound take1Effect;
Sound chipEffect;
Sound spinStartEffect;
Sound spinEndEffect;

int money = 0;

int ballCount  = 1;
int speed      = 0;
int moneyMult  = 1;
int collisions = 0;
int level = 0;

bool seeFirstCard = false;
bool retainLoss   = false;
bool swapHands    = false;
bool goldBall     = false;

bool lockBet     = false;

float bonusTimer = 0;
int previewIndex = -1;

#include "utils.h"
#include "blackjack.h"
#include "game.h"
#include "slot.h"
#include "roulette.h"

void DrawBets(int usable) {
    if(playerBet > 0 && usable){
        if(Red_Button((Rectangle){ 1130 , 260, 50, 50 }, "X", (Color){160,40,20,255})) {
            money += playerBet;
            playerBet = 0;
            lockBet = false;
            PlaySound(chipEffect);
        }
    }

    int betValues[] = {50, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000};
    const char *betLabels[] = { "50", "100", "500", "1K", "5K", "10K", "50K", "100K", "500K", "1M" };

    int xStart = 745;
    int yStart = 375;

    for (int i = 0; i < 10; i++) {
        int columns = 5;
        int chipWidth = 72;
        int chipHeight = 72;
        int xPaddingPerChip = 8;
        int yPaddingPerRow = 5;

        int col = i % columns;
        int row = i / columns;

        int xOffset = col * chipWidth;
        int yOffset = row * chipHeight;

        int paddingX = col * xPaddingPerChip;
        int paddingY = row * yPaddingPerRow;

        Rectangle destRect = {
            (float)xStart + xOffset + paddingX,
            (float)yStart + yOffset + paddingY,
            (float)chipWidth,
            (float)chipHeight
        };

        Rectangle buttonRect = {
             destRect.x + 2,
             destRect.y + 2,
             destRect.width - 4,
             destRect.height - 4
        };

        Rectangle sourceRect = {
            (float)xOffset + row * 360,
            (float)0,
            (float)chipWidth,
            (float)chipHeight
        };

        if (money >= betValues[i] && usable) {
             if (Empty_Button(buttonRect)) {
                money -= betValues[i];
                playerBet += betValues[i];
                PlaySound(chipEffect);
             }
             DrawTexturePro(chipTexture, sourceRect, destRect, (Vector2){ 0, 0 }, 0, WHITE);
        } else {
            DrawTexturePro(chipTexture, sourceRect, destRect, (Vector2){ 0, 0 }, 0, (Color){80, 80, 80, 255});
        }

        Rectangle textBounds = { destRect.x, destRect.y, destRect.width, destRect.height};
        DrawTextCentered(font26, betLabels[i], textBounds, 26, BLACK);
    }
}

void UpdateDrawFrame();

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(screenWidth, screenHeight, "IDLE GAMBLING");
    InitAudioDevice();

    font120 = LoadFontEx("assets/ChakraPetch-Bold.ttf", 120, 0, 1024);
    font96  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 96, 0, 1024);
    font80  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 80, 0, 1024);
    font64  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 64, 0, 1024);
    font48  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 48, 0, 1024);
    font44  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 44, 0, 1024);
    font36  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 36, 0, 1024);
    font33  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 33, 0, 1024);
    font30  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 30, 0, 1024);
    font26  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 26, 0, 1024);
    font24  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 24, 0, 1024);
    font18  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 18, 0, 1024);
    font16  = LoadFontEx("assets/ChakraPetch-Bold.ttf", 16, 0, 1024);

    chipTexture = LoadTexture("assets/chips.png");
    deckTexture = LoadTexture("assets/deck.png");
    backTexture = LoadTexture("assets/cardBack.png");
    lockTexture = LoadTexture("assets/lock.png");
    slotMachineTexture = LoadTexture("assets/machine.png");
    slotSpriteSheet = LoadTexture("assets/slot2.png");
    trFlag = LoadTexture("assets/tr.png");
    enFlag = LoadTexture("assets/en.png");
    logoTexture = LoadTexture("assets/logo.png");
    backgroundPattern = LoadTexture("assets/background.png");

    blueButtonTexture = LoadTexture("assets/bluebtn.png");
    redButtonTexture = LoadTexture("assets/redbtn.png");
    greenButtonTexture = LoadTexture("assets/greenbtn.png");

    tables[0] = LoadTexture("assets/tables/table1.png");
    tables[1] = LoadTexture("assets/tables/table2.png");
    tables[2] = LoadTexture("assets/tables/table3.png");
    tables[3] = LoadTexture("assets/tables/table4.png");
    tables[4] = LoadTexture("assets/tables/table5.png");

    take4Effect = LoadSound("assets/take4.wav");
    take1Effect = LoadSound("assets/take1.wav");
    chipEffect = LoadSound("assets/chip.wav");
    spinStartEffect = LoadSound("assets/spinstart.wav");
    spinEndEffect   = LoadSound("assets/spinend.wav");

    SetTextureWrap(slotSpriteSheet, TEXTURE_WRAP_REPEAT);
    SetTextureWrap(backgroundPattern, TEXTURE_WRAP_REPEAT);

    LoadSave();
    money          = save.money;
    ballCount      = save.ballCount;
    speed          = save.speed;
    moneyMult      = save.moneyMult;
    level          = save.level;
    seeFirstCard   = save.seeFirstCard;
    retainLoss     = save.retainLoss;
    swapHands      = save.swapHands;
    goldBall       = save.goldBall;

    SetMasterVolume(save.volume);
    InitGame();

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else

        while (!WindowShouldClose())
        {
            UpdateDrawFrame();
        }
    #endif

    SaveState();
    CloseWindow();
}

void UpdateDrawFrame()
{
    BeginDrawing();
        ClearBackground((Color){ 0, 0, 0, 255 });
        DrawRectangleGradientV(0, 0, 1200, 540, (Color) {1, 76, 14, 255}, (Color) {0, 95, 12, 255});

        Rectangle destRect = {
            (float)-300,
            (float)-300,
            (float)2048,
            (float)2048
        };

        Rectangle sourceRect = {
            (float)GetTime() * -20,
            (float)GetTime() * 20,
            (float)backgroundPattern.width,
            (float)backgroundPattern.height
        };

        DrawTexturePro(backgroundPattern, sourceRect, destRect, (Vector2){ 0, 0 }, 10, WHITE);

        switch(currentScreen)
        {
            case TITLE:
            {
                DrawTextureEx(logoTexture, (Vector2){390, -30}, 0, 0.4,  WHITE);

                if(Red_Button((Rectangle){ 600, 340, 240, 70 }, TEXT_PLAY, (Color){ 143, 52, 30, 255 }))
                    currentScreen = GAMEPLAY;
                if(Red_Button((Rectangle){ 475, 340 + 80, 240, 70 }, TEXT_SETTINGS, (Color){ 143, 52, 30, 255 }))
                    currentScreen = SETTINGS;
                if(Red_Button((Rectangle){ 350, 340, 240, 70 }, TEXT_SHOP, (Color){ 143, 52, 30, 255 }))
                {
                    currentScreen = SHOP;
                    previewIndex = save.currentCosmetic;
                }

                DrawTexture(enFlag, 20, 460, WHITE);
                DrawTexture(trFlag, 20 + 64 + 20, 460, WHITE);

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 20, 460, 64, 64}))
                {
                    save.lang = 0;
                    SaveState();
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 20 + 64 + 20, 460, 64, 64}))
                {
                    save.lang = 1;
                    SaveState();
                }
            } break;

            case SETTINGS:
            {
                DrawTextCentered(font120, TEXT_SETTINGS, (Rectangle){0, 0, 1200, 150}, 120, WHITE);

                DrawTextCentered(font48, TEXT_VOLUME, (Rectangle){600, 250, 200, 0}, 48, WHITE);
                save.volume = VolumeSlider((Rectangle){600, 307, 200, 15}, save.volume);

                DrawTextCentered(font48, TEXT_VIBRATION, (Rectangle){500 - 35, 250, 70, 0}, 48, WHITE);
                save.vibration = VibrationToggle((Rectangle){500 - 35, 300, 60, 30}, save.vibration);

                if (Red_Button((Rectangle){1125, 25, 50, 50}, "X", (Color){143, 52, 30, 255})) {
                    SaveState();
                    currentScreen = TITLE;
                }

                DrawTextCentered(font48, "Cheats", (Rectangle){10, 20, 150, 50}, 48, WHITE);

                if(Red_Button((Rectangle){ 10, 80, 150, 50 }, "Bonus", (Color){ 143, 52, 30, 255 })) {
                    bonusTimer = 6 * 60;
                }

                if(Red_Button((Rectangle){ 10, 140, 150, 50 }, "Money", (Color){ 143, 52, 30, 255 })) {
                    money += 1000000;
                    SaveState();
                }

                SetMasterVolume(save.volume);

            } break;

            case SHOP:
            {
                DrawTextCentered(font80, TEXT_SHOP, (Rectangle){450, 20, 800, 80}, 80, WHITE);
                DrawTextCentered(font64, FormatMoney(money), (Rectangle){100, 60, 380, 0}, 64, WHITE);

                int startX = 100;
                int startY = 110;
                int btnWidth = 240;
                int btnHeight = 60;
                int spacing = 10;

                for (int i = 0; i < MAX_COSMETICS; i++) {
                    Cosmetic* cos = &cosmetics[i];

                    bool isOwned = (save.ownedCosmetics & (1 << cos->bitIndex)) != 0;
                    bool isEquipped = (save.currentCosmetic == cos->bitIndex);
                    bool isPreviewed = (previewIndex == i);

                    Rectangle previewBtn = {startX, startY + i * (btnHeight + spacing), btnWidth, btnHeight};
                    Rectangle actionBtn = {previewBtn.x + btnWidth + 10, previewBtn.y, 160, btnHeight};
                    if(save.lang) {
                       if (Red_Button(previewBtn,cos->nameEN, (Color){ 143, 52, 30, 255 })) {
                            previewIndex = i;
                        }
                    } else {
                       if (Red_Button(previewBtn, cos->nameTR, (Color){ 143, 52, 30, 255 })) {
                            previewIndex = i;
                        }
                    }


                    if (!isOwned) {
                        if (Red_Button(actionBtn, FormatMoney(cos->price), (Color){143, 52, 30, 255})) {
                            if (save.money >= cos->price) {
                                save.money -= cos->price;
                                save.ownedCosmetics |= (1 << cos->bitIndex);
                                PlaySound(chipEffect);
                                SaveState();
                            }
                        }
                    } else {
                        if (Green_Button(actionBtn, isEquipped ? TEXT_EQUIPPED : TEXT_EQUIP, (Color){60, 140, 0, 255})) {
                            save.currentCosmetic = cos->bitIndex;
                            previewIndex = i;
                            PlaySound(chipEffect);
                            SaveState();
                        }
                    }

                    if (isPreviewed) {
                        DrawRectangleRoundedLinesEx(previewBtn, 0.3f, 32, 4, GOLD);
                    }
                }

                Rectangle sourceRect = { 0,0, tables[previewIndex].width, tables[previewIndex].height };
                DrawTexturePro(tables[previewIndex], sourceRect, (Rectangle){ 550, 110, 600, 350 }, (Vector2){ 0, 0 }, 0, WHITE);

                static const Ball ball = {(Vector2){1050, 360}};

                DrawCircleV((Vector2){ball.pos.x + 3, ball.pos.y + 3}, 15, ColorAlpha(BLACK, 0.2f));
                DrawCircleV(ball.pos, 15, RED);
                DrawCircleGradient(ball.pos.x - 15 * 0.3f, ball.pos.y - 15*0.3f, 15*0.3f, ColorAlpha(WHITE, 0.7f), ColorAlpha(WHITE, 0.6f));

                if (Red_Button((Rectangle){1125, 25, 50, 50}, "X", (Color){143, 52, 30, 255})) {
                    SaveState();
                    currentScreen = TITLE;
                }
            }
            break;

            case GAMEPLAY:
            {
                static int currentGame = 0;

                float deltaTime = Clamp(GetFrameTime(), 1.0f / 165.0f, 1.0f / 60.0f);

                if(Blue_Button((Rectangle){ 1110, 10, 70, 50 }, "| |", BUTTON_COLOR)) {
                    currentScreen = TITLE;
                }

                UpdateGame(deltaTime);

                if(!levelingUp) {
                    if(!spinning && bjState == Main && currentRouletteState == ROULETTE_STATE_BETTING && Blue_Button((Rectangle){ 1130, 90, 50, 50 }, ">", BUTTON_COLOR)) {
                        currentGame = (currentGame + 1) % 3;
                    }

                    if(currentGame == 0) {
                        if(bjState == Main) {
                            DrawBets(1);
                        }

                        UpdateBlackJack(deltaTime);
                    }

                    if(currentGame == 1) {
                        DrawBets(currentRouletteState == ROULETTE_STATE_BETTING);

                        UpdateDrawRoulette(deltaTime);
                    }

                    if(currentGame == 2) {
                        DrawBets(!spinning);

                        UpdateSlot(deltaTime);
                    }
                }
            } break;

            default: break;
        }

    EndDrawing();
}
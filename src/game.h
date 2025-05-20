typedef struct {
    Vector2 pos;
    Vector2 vel;
} Ball;

#define MAX_BALLS 128
Ball balls[MAX_BALLS];

int ballPrices[]  = { 25, 250, 500, 1000, 2500, 5000, 7500, 10000, 15000, 20000, 30000, 50000, 75000, 100000, 150000, 200000, 300000, 400000, 500000, 600000, 750000, 1000000, 1500000, 2000000, 2500000, -1};
int speedPrices[] = { 50, 250, 500, 1000, 2500, 5000, 7500, 10000, 15000, 20000, 30000, 50000, 75000, 100000, 150000, 200000, 300000, 400000, 500000, 600000, 750000, 1000000, 1500000, 2000000, 2500000, -1};
int multPrices[]  = { 100, 500, 1000, 2500, 5000, 7500, 10000, 25000, 30000, 40000, 60000, 80000, 150000, 200000, 300000, 500000, 750000, 1000000, 1500000, 2000000, 2500000, 3000000, -1};
int levelPrices[] = { 250000, 1000000, 3000000, 5000000, -1};

float timer = 0;
bool levelingUp = false;

Vector2 swipeStart = { 0.0f, 0.0f };
Vector2 swipeEnd = { 0.0f, 0.0f };
Vector2 swipeVector = { 0.0f, 0.0f };
bool isSwiping = false;
bool swipeDetected = false;
float lastSwipeTime = -10.0;
float swipeCooldown = 0.5;

int SimilationWidth = 700;
int SimilationHeight = 420;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    int amount;
    float lifetime;
    float alpha;
    Color color;
    bool active;
} FadingText;

#define MAX_FADING_TEXTS 64
FadingText fadingTexts[MAX_FADING_TEXTS];

const float FADE_TEXT_DURATION = 0.75f;
const Color FADE_TEXT_COLOR = {0, 228, 48, 255};

void SpawnParticle(Vector2 position, int amount, Color color, float duration) {

    if (amount <= 0) return;

    for (int i = 0; i < MAX_FADING_TEXTS; i++) {
        if (!fadingTexts[i].active) {
            fadingTexts[i].pos = position;
            fadingTexts[i].vel = (Vector2){ GetRandomValue(-30, +30), GetRandomValue(-60, -90)};
            fadingTexts[i].amount = amount;
            fadingTexts[i].lifetime = duration;
            fadingTexts[i].alpha = 1.0f;
            fadingTexts[i].color = color;
            fadingTexts[i].active = true;
            return;
        }
    }
}

void UpdateParticles(float deltaTime) {
    for (int i = 0; i < MAX_FADING_TEXTS; i++) {
        if (fadingTexts[i].active) {
            fadingTexts[i].lifetime -= deltaTime;

            if (fadingTexts[i].lifetime <= 0) {
                fadingTexts[i].active = false;
                continue;
            }

            fadingTexts[i].vel.y += 20 * deltaTime;

            fadingTexts[i].pos.x += fadingTexts[i].vel.x * deltaTime;
            fadingTexts[i].pos.y += fadingTexts[i].vel.y * deltaTime;

            //fadingTexts[i].pos.y -= FADE_TEXT_FLOAT_SPEED * deltaTime;

            float initialDuration = FADE_TEXT_DURATION > 0 ? FADE_TEXT_DURATION : 1.0f;
            fadingTexts[i].alpha = Clamp(fadingTexts[i].lifetime / initialDuration, 0.0f, 1.0f);

            Color drawColor = ColorAlpha(fadingTexts[i].color, fadingTexts[i].alpha);

            Vector2 textSize = MeasureTextEx(font24, TextFormat("+%d$", fadingTexts[i].amount), 24, 1);
            DrawTextEx(font24, TextFormat("+%d$", fadingTexts[i].amount), (Vector2){fadingTexts[i].pos.x - textSize.x / 2, fadingTexts[i].pos.y}, 24, 1, drawColor);
        }
    }
}

#include "fballs.h"

void InitGame() {
    fBallInit();
    for (int i = 0; i < ballCount; i++) {
        balls[i].pos = (Vector2){300 + GetRandomValue(-150, 150), 240 + GetRandomValue(-120, 120)};
        balls[i].vel = (Vector2){ GetRandomValue(-100, 100), GetRandomValue(-100, 100) };
    }
    InitBlackJack();
}

void DrawProgressBar(float percentage, int x, int y, int width, int height) {
    DrawRectangleRounded((Rectangle){ x, y, width, height }, 1.0f, 16, (Color){ 111, 68, 26, 255 });

    int pixels = Clamp(percentage, 0.0f, 1.0f) * (float)width;
    DrawRectangleRounded((Rectangle){ x, y, pixels, height }, 1.0f, 16, (Color){ 28, 137, 35, 255 });

    DrawTextCentered(font24, FormatMoney(levelPrices[level]), (Rectangle){x, y + 18, width, height }, 24, WHITE);
}

void UpdateGame(float deltaTime) {
    timer += GetFrameTime();
    if(timer > 3.0f) {
        save.money          = money;
        save.ballCount      = ballCount;
        save.speed          = speed;
        save.moneyMult      = moneyMult;
        save.level = level;
        save.seeFirstCard   = seeFirstCard;
        save.retainLoss     = retainLoss;
        save.swapHands      = swapHands;
        save.goldBall       = goldBall;
        SaveState();
        timer = 0;
    }

    Rectangle sourceRect = { 0,0, tables[save.currentCosmetic].width, tables[save.currentCosmetic].height };
    DrawTexturePro(tables[save.currentCosmetic], sourceRect, (Rectangle){ 0, 0, SimilationWidth, SimilationHeight }, (Vector2){ 0, 0 }, 0, WHITE);

    int buttonCount = 3;
    int buttonWidth = 210;
    int screenWidth = 700;
    int spacing = (screenWidth - (buttonCount * buttonWidth)) / (buttonCount + 1); // Spacing between buttons

    int x1 = spacing;
    int x2 = x1 + buttonWidth + spacing;
    int x3 = x2 + buttonWidth + spacing;

    if (ballPrices[ballCount - 1] != -1) {
        DrawTextCentered(font36, FormatMoney(ballPrices[ballCount - 1]), (Rectangle){ x1, 460 - 45, buttonWidth, 50 }, 36, WHITE);
        if (Blue_Button((Rectangle){ x1, 460, buttonWidth, 65 }, TEXT_BALL, BUTTON_COLOR)) {
            if (money > ballPrices[ballCount - 1]) {
                money -= ballPrices[ballCount - 1];
                balls[ballCount].pos = (Vector2){ 300 + GetRandomValue(-150, 150), 240 + GetRandomValue(-120, 120) };
                balls[ballCount].vel = (Vector2){ GetRandomValue(-100, 100), GetRandomValue(-100, 100) };
                ballCount++;
            }
        }
    } else {
        DrawTextCentered(font36, TEXT_BALL, (Rectangle){ x1, 460 - 45, buttonWidth, 50 }, 36, WHITE);
        DrawRectangleRounded((Rectangle){ x1, 460, buttonWidth, 65 }, 0.25f, 16, (Color){ 25, 60, 120, 255 });
        DrawTextCentered(font36, TEXT_SOLDOUT, (Rectangle){ x1, 460, buttonWidth, 65 }, 36, GRAY);
    }

    if (speedPrices[speed] != -1) {
        DrawTextCentered(font36, FormatMoney(speedPrices[speed]), (Rectangle){ x2, 460 - 45, buttonWidth, 50 }, 36, WHITE);
        if (Blue_Button((Rectangle){ x2, 460, buttonWidth, 65 }, TEXT_SPEED, BUTTON_COLOR)) {
            if (money > speedPrices[speed]) {
                money -= speedPrices[speed];
                speed++;
            }
        }
    } else {
        DrawTextCentered(font36, TEXT_SPEED, (Rectangle){ x2, 460 - 45, buttonWidth, 50 }, 36, WHITE);
        DrawRectangleRounded((Rectangle){ x2, 460, buttonWidth, 65 }, 0.25f, 16, (Color){ 25, 60, 120, 255 });
        DrawTextCentered(font36, TEXT_SOLDOUT, (Rectangle){ x2, 460, buttonWidth, 65 }, 36, GRAY);
    }

    if (multPrices[moneyMult - 1] != -1) {
        DrawTextCentered(font36, FormatMoney(multPrices[moneyMult - 1]), (Rectangle){ x3, 460 - 45, buttonWidth, 50 }, 36, WHITE);
        if (Blue_Button((Rectangle){ x3, 460, buttonWidth, 65 }, TEXT_MULTIPLIER, BUTTON_COLOR)) {
            if (money > multPrices[moneyMult - 1]) {
                money -= multPrices[moneyMult - 1];
                moneyMult++;
            }
        }
    } else {
        DrawTextCentered(font36, TEXT_MULTIPLIER, (Rectangle){ x3, 460 - 45, buttonWidth, 50 }, 36, WHITE);
        DrawRectangleRounded((Rectangle){ x3, 460, buttonWidth, 65 }, 0.25f, 16, (Color){ 25, 60, 120, 255 });
        DrawTextCentered(font36, TEXT_SOLDOUT, (Rectangle){ x3, 460, buttonWidth, 65 }, 36, GRAY);
    }

    if(levelPrices[level] > 0) {
        float percentage = (float)money / (float)levelPrices[level];
        DrawProgressBar(percentage, SimilationWidth + 20, 70, (1200 - SimilationWidth) - 40, 10);
        if(percentage > 1.0f) {
            if(levelingUp) {
                if(Blue_Button((Rectangle){ 980, 10, 120, 50 }, TEXT_CANCEL, BUTTON_COLOR)) {
                    levelingUp = false;
                }
            } else {
                if(Blue_Button((Rectangle){ 980, 10, 120, 50 }, TEXT_LEVELUP, BUTTON_COLOR)) {
                    levelingUp = true;
                }
            }
        }
    }

    for (int i = 0; i < ballCount; i++) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, (Rectangle){0, 0, SimilationWidth, SimilationHeight})) {
                swipeStart = mousePos;
                isSwiping = true;
                swipeDetected = false;
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isSwiping) {
            swipeEnd = GetMousePosition();
            swipeVector = (Vector2){ swipeEnd.x - swipeStart.x, swipeEnd.y - swipeStart.y };
            isSwiping = false;
            if(Vector2Length(swipeVector) > 10) {
                swipeDetected = true;
                lastSwipeTime = GetTime();
            }
        }

        if (swipeDetected) {
            for (int i = 0; i < ballCount; i++) {
                balls[i].vel = Vector2Normalize(swipeVector);
            }
            swipeDetected = false;
        }

        float ballSpeed = speed * 100 + 400;
        balls[i].vel = Vector2Scale(Vector2Normalize(balls[i].vel), ballSpeed);
        balls[i].pos = Vector2Add(balls[i].pos, Vector2Scale(balls[i].vel, deltaTime));

        const int radius = 15;
        const int padding = 20;

        bool collidedWall = false;

        if (balls[i].pos.y <= (radius + padding)) {
            balls[i].pos.y =  (radius + padding);
            balls[i].vel.y *= -1;
            collidedWall = true;
        }

        if (balls[i].pos.y >= SimilationHeight - (radius + padding)) {
            balls[i].pos.y = SimilationHeight - (radius + padding);
            balls[i].vel.y *= -1;
            collidedWall = true;
        }

        if (balls[i].pos.x <= (radius + padding)) {
            balls[i].pos.x = (radius + padding);
            balls[i].vel.x *= -1;
            collidedWall = true;
        }

        if (balls[i].pos.x >= SimilationWidth - (radius + padding)) {
            balls[i].pos.x = SimilationWidth - (radius + padding);
            balls[i].vel.x *= -1;
            collidedWall = true;
        }

        if (collidedWall) {
            int moneyEarned = (goldBall && i < 2) ? moneyMult * 2 : moneyMult;
            money += moneyEarned;
            collisions++;

            Vector2 textPos = balls[i].pos;
            textPos.y -= radius;
            SpawnParticle(textPos, moneyEarned, FADE_TEXT_COLOR, FADE_TEXT_DURATION);
        }

        for (int j = i + 1; j < ballCount; j++) {
            Vector2 dir = Vector2Subtract(balls[j].pos, balls[i].pos);
            float d = Vector2Length(dir);
            if (d == 0.0f || d > 28.0f) {
                continue;
            }

            dir = Vector2Scale(dir, 1.0f / d);

            float corr = 1;
            balls[i].pos = Vector2Add(balls[i].pos, Vector2Scale(dir, -corr));
            balls[j].pos = Vector2Add(balls[j].pos, Vector2Scale(dir, corr));

            float v1 = Vector2DotProduct(balls[i].vel, dir);
            float v2 = Vector2DotProduct(balls[j].vel, dir);

            balls[i].vel = Vector2Add(balls[i].vel, Vector2Scale(dir, v2 - v1));
            balls[j].vel = Vector2Add(balls[j].vel, Vector2Scale(dir, v1 - v2));

            int moneyEarned = (goldBall && i < 2) ? moneyMult * 2 : moneyMult;
            money += moneyEarned;
            collisions++;

            SpawnParticle((Vector2){(balls[i].pos.x + balls[j].pos.x) / 2,( balls[i].pos.y + balls[j].pos.y) / 2}, moneyEarned, FADE_TEXT_COLOR, FADE_TEXT_DURATION);
        }


        if(goldBall && i < 2) {
            DrawCircleV((Vector2){balls[i].pos.x + 3, balls[i].pos.y + 3}, radius, ColorAlpha(BLACK, 0.2f));
            DrawCircleV(balls[i].pos, radius, (Color) {255, 255, 0, 255});
        } else {
            DrawCircleV((Vector2){balls[i].pos.x + 3, balls[i].pos.y + 3}, radius, ColorAlpha(BLACK, 0.2f));
            DrawCircleV(balls[i].pos, radius, RED);
            DrawCircleGradient(balls[i].pos.x - radius*0.3f, balls[i].pos.y - radius*0.3f, radius*0.3f, ColorAlpha(WHITE, 0.7f), ColorAlpha(WHITE, 0.6f));

            //DrawCircleGradient(balls[i].pos.x, balls[i].pos.y, 15, (Color) {240, 100, 100, 255}, (Color) {240, 10, 10, 255});
        }
    }

    DrawTextEx(font48, FormatMoney(money), (Vector2){SimilationWidth + 15, 10}, 48, 0, WHITE);

    if(levelingUp) {
        int currentY = 170;

        if(!seeFirstCard) {
            DrawTextCentered(font36, TEXT_SEEFIRST_DESC, (Rectangle){ 860, currentY - 60, 180, 60 }, 36, WHITE);
            if(Blue_Button((Rectangle){ 860, currentY, 180, 60 }, TEXT_SEEFIRST, BUTTON_COLOR)) {
                money      = 0;
                ballCount  = 1;
                speed      = 0;
                moneyMult  = 1;
                collisions = 0;
                seeFirstCard = true;
                levelingUp   = false;
                playerBet = 0;
                bjState = Main;
                bonusTimer = 0;
                level += 1;
            }
            currentY += 140;
        } else if(!swapHands) {
            DrawTextCentered(font36, TEXT_SWAP_DESC, (Rectangle){ 860, currentY - 60, 180, 60 }, 36, WHITE);
            if(Blue_Button((Rectangle){ 860, currentY, 180, 60 }, TEXT_SWAP, BUTTON_COLOR)) {
                money      = 0;
                ballCount  = 1;
                speed      = 0;
                moneyMult  = 1;
                collisions = 0;
                swapHands = true;
                levelingUp = false;
                playerBet = 0;
                bjState = Main;
                bonusTimer = 0;
                level += 1;
            }
            currentY += 140;
        }

        if(!retainLoss) {
            DrawTextCentered(font36, TEXT_RETAINLOSS_DESC, (Rectangle){ 860, currentY - 60, 180, 60 }, 36, WHITE);
            if(Blue_Button((Rectangle){ 860, currentY, 180, 60 }, TEXT_RETAINLOSS, BUTTON_COLOR)) {
                money      = 0;
                ballCount  = 1;
                speed      = 0;
                moneyMult  = 1;
                collisions = 0;

                retainLoss = true;
                levelingUp = false;
                playerBet = 0;
                bjState = Main;
                bonusTimer = 0;
                level += 1;
            }
            currentY += 140;
        }

        if(!goldBall) {
            DrawTextCentered(font36, TEXT_GOLDEN_DESC, (Rectangle){ 860, currentY - 60, 180, 60 }, 36, WHITE);
            if(Blue_Button((Rectangle){ 860, currentY, 180, 60 }, TEXT_GOLDEN, BUTTON_COLOR)) {
                money      = 0;
                ballCount  = 1;
                speed      = 0;
                moneyMult  = 1;
                collisions = 0;

                goldBall = true;
                levelingUp = false;
                playerBet = 0;
                bjState = Main;
                bonusTimer = 0;
                level += 1;
            }
            currentY += 120;
        }

    }

    UpdateParticles(deltaTime);
    fBallUpdate(deltaTime);
}
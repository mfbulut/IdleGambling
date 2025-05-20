#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define NUM_ROULETTE_NUMBERS 37
#define ROULETTE_WHEEL_RADIUS 120.0f
#define ROULETTE_BALL_RADIUS 5.0f
#define ROULETTE_CENTER_X 840
#define ROULETTE_CENTER_Y 230
#define MAX_ROULETTE_SPEED 600.0f
#define MAX_BALL_SPEED_MULTIPLIER 1.5f
#define SPIN_DURATION_ROULETTE 5.0f
#define DECELERATION_ROULETTE 100.0f
#define BALL_DECELERATION_FACTOR 1.1f
#define RESULT_DISPLAY_TIME 4.0f

#define MANUAL_SPIN_DAMPING 0.8f
#define MAX_MANUAL_SPIN_SPEED 400.0f

Color ROULETTE_GREEN = { 0, 150, 0, 255 };
Color ROULETTE_RED = { 200, 0, 0, 255 };
Color ROULETTE_BLACK = { 30, 30, 30, 255 };
Color ROULETTE_GOLD = { 255, 215, 0, 255 };

const int rouletteNumbers[NUM_ROULETTE_NUMBERS] = {
    0, 32, 15, 19, 4, 21, 2, 25, 17, 34, 6, 27, 13, 36, 11, 30, 8, 23, 10,
    5, 24, 16, 33, 1, 20, 14, 31, 9, 22, 18, 29, 7, 28, 12, 35, 3, 26
};

typedef enum {
    ROULETTE_STATE_BETTING,
    ROULETTE_STATE_SPINNING,
    ROULETTE_STATE_BALL_LANDING,
    ROULETTE_STATE_RESULT
} RouletteState;

RouletteState currentRouletteState = ROULETTE_STATE_BETTING;
float rouletteAngle = 0.0f;
float rouletteSpeed = 0.0f;
float ballAngle = 4.8f;
float ballSpeed = 0.0f;
float ballDistanceFromCenter = ROULETTE_WHEEL_RADIUS * 0.8f;

int winningNumber = -1;
long long betOnRed = 0;
long long betOnBlack = 0;
long long betOnGreen = 0;
long long totalBetThisRound = 0;
long long winningsThisRound = 0;

float rouletteTimer = 0.0f;

Color GetRouletteNumberColor(int number);
void DrawRouletteWheel(Vector2 center, float radius, float rotation);
void CalculateRouletteWinnings();
void ResetRouletteRound();

void UpdateDrawRoulette(float deltaTime) {
    const float buttonX = 1000;
    const float buttonY = 140;
    const float buttonHeight = 50;
    const float buttonWidth = 100;
    const float buttonSpacing = 10;

    Rectangle greenButtonRect = { buttonX, buttonY, buttonWidth, buttonHeight };
    Rectangle blackButtonRect = { buttonX, buttonY + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };
    Rectangle redButtonRect   = { buttonX, buttonY+ 2 * (buttonHeight + buttonSpacing), buttonWidth, buttonHeight };

    static bool isDraggingWheel = false;
    static float firstMouseAngle = 0.0f;
    static float lastMouseAngle = 0.0f;
    static float lastFrameDragSpeed = 0.0f;

    if(currentRouletteState == ROULETTE_STATE_BETTING && !lockBet && playerBet > 0){
        if(Blue_Button((Rectangle){ 1130, 200, 50, 50 }, "", BUTTON_COLOR)) {
            lockBet = true;
        }

        DrawTexturePro(
            lockTexture,
            (Rectangle){0, 0, lockTexture.width, lockTexture.height},
            (Rectangle){1130 + 12, 200 + 7, 30, 30},
            (Vector2){0, 0},
            0,
            WHITE
        );
    }

    Vector2 mousePos = GetMousePosition();
    Vector2 wheelCenter = { ROULETTE_CENTER_X, ROULETTE_CENTER_Y };
    float distFromCenter = Vector2Distance(mousePos, wheelCenter);

    switch (currentRouletteState) {
        case ROULETTE_STATE_BETTING:
            {
                bool spinStarted = false;

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && distFromCenter <= ROULETTE_WHEEL_RADIUS * 1.05f) {
                    // Check if click is NOT on a betting button
                    if (!CheckCollisionPointRec(mousePos, greenButtonRect) &&
                        !CheckCollisionPointRec(mousePos, blackButtonRect) &&
                        !CheckCollisionPointRec(mousePos, redButtonRect))
                    {
                         isDraggingWheel = true;
                         lastMouseAngle = atan2f(mousePos.y - wheelCenter.y, mousePos.x - wheelCenter.x) * RAD2DEG;
                         firstMouseAngle = lastMouseAngle;
                         rouletteSpeed = 0;
                         lastFrameDragSpeed = 0;
                    }
                }

                if (isDraggingWheel && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    float currentMouseAngle = atan2f(mousePos.y - wheelCenter.y, mousePos.x - wheelCenter.x) * RAD2DEG;
                    float deltaAngle = currentMouseAngle - lastMouseAngle;

                    if (deltaAngle > 180.0f) deltaAngle -= 360.0f;
                    if (deltaAngle < -180.0f) deltaAngle += 360.0f;

                    rouletteAngle += deltaAngle;
                    ballAngle += deltaAngle;

                    rouletteAngle = fmodf(rouletteAngle, 360.0f);
                    if (rouletteAngle < 0) rouletteAngle += 360.0f;

                    lastFrameDragSpeed = (lastMouseAngle - firstMouseAngle) * 20;

                    lastMouseAngle = currentMouseAngle;
                }

                if (isDraggingWheel && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    isDraggingWheel = false;
                    // Apply inertia based on the last drag movement
                    rouletteSpeed = lastFrameDragSpeed * MANUAL_SPIN_DAMPING;
                    // Clamp speed
                    if (rouletteSpeed > MAX_MANUAL_SPIN_SPEED) rouletteSpeed = MAX_MANUAL_SPIN_SPEED;
                    if (rouletteSpeed < -MAX_MANUAL_SPIN_SPEED) rouletteSpeed = -MAX_MANUAL_SPIN_SPEED;
                }

                rouletteSpeed *= 0.95f;
                rouletteAngle += rouletteSpeed * deltaTime;

                ballAngle += 1.8f * rouletteSpeed  * deltaTime;

                if (Blue_Button(redButtonRect, "RED", ROULETTE_RED)) {
                    if (playerBet > 0) {
                        ResetRouletteRound();
                        betOnRed = playerBet;
                        totalBetThisRound = playerBet;

                        spinStarted = true;
                    }
                }
                else if (Blue_Button(blackButtonRect, "BLACK", ROULETTE_BLACK)) {
                     if (playerBet > 0) {
                        ResetRouletteRound();
                        betOnBlack = playerBet;
                        totalBetThisRound = playerBet;

                        spinStarted = true;
                    }
                }
                else if (Blue_Button(greenButtonRect, "GREEN", ROULETTE_GREEN)) {
                     if (playerBet > 0) {
                        ResetRouletteRound();
                        betOnGreen = playerBet;
                        totalBetThisRound = playerBet;
                        spinStarted = true;
                    }
                }


                if (spinStarted) {
                    currentRouletteState = ROULETTE_STATE_SPINNING;
                    rouletteTimer = 0.0f;
                    winningNumber = -1;
                    winningsThisRound = 0;

                    rouletteSpeed = GetRandomValue(MAX_ROULETTE_SPEED * 0.8f, MAX_ROULETTE_SPEED);
                    if (GetRandomValue(0, 1)) rouletteSpeed *= -1;
                    ballSpeed = -rouletteSpeed * GetRandomValue(110, 120) / 100.0f;
                    ballDistanceFromCenter = ROULETTE_WHEEL_RADIUS * 0.85f;
                    // PlaySound(spinSoundRoulette);
                }
            }
            break;

        case ROULETTE_STATE_SPINNING:
            {
                rouletteTimer += deltaTime;
                rouletteAngle += rouletteSpeed * deltaTime;
                ballAngle += ballSpeed * deltaTime;

                if (rouletteSpeed > 0) {
                    rouletteSpeed -= DECELERATION_ROULETTE * deltaTime;
                    if (rouletteSpeed < 0) rouletteSpeed = 0;
                } else if (rouletteSpeed < 0) {
                    rouletteSpeed += DECELERATION_ROULETTE * deltaTime;
                    if (rouletteSpeed > 0) rouletteSpeed = 0;
                }

                if (ballSpeed > 0) {
                    ballSpeed -= DECELERATION_ROULETTE * BALL_DECELERATION_FACTOR * deltaTime;
                    if (ballSpeed < 0) ballSpeed = 0;
                } else if (ballSpeed < 0) {
                    ballSpeed += DECELERATION_ROULETTE * BALL_DECELERATION_FACTOR * deltaTime;
                    if (ballSpeed > 0) ballSpeed = 0;
                }

                if (ballDistanceFromCenter > ROULETTE_BALL_RADIUS * 2) {
                    ballDistanceFromCenter -= deltaTime * 7.0f * (1.0f - (fabsf(ballSpeed) / (MAX_ROULETTE_SPEED * MAX_BALL_SPEED_MULTIPLIER)));
                }

                rouletteAngle = fmodf(rouletteAngle, 360.0f);
                ballAngle = fmodf(ballAngle, 360.0f);

                if (rouletteTimer > SPIN_DURATION_ROULETTE && fabsf(ballSpeed) < 1.5f && fabsf(ballSpeed) < 60.0f) {
                    currentRouletteState = ROULETTE_STATE_BALL_LANDING;
                    // PlaySound(ballClickSound);
                }
            }
            break;

        case ROULETTE_STATE_BALL_LANDING:
             {
                rouletteAngle += rouletteSpeed * deltaTime;
                ballAngle += ballSpeed * deltaTime;

                if (rouletteSpeed > 0) rouletteSpeed -= DECELERATION_ROULETTE * 0.5f * deltaTime;
                else if (rouletteSpeed < 0) rouletteSpeed += DECELERATION_ROULETTE * 0.5f * deltaTime;

                if (ballSpeed > 0) ballSpeed -= DECELERATION_ROULETTE * BALL_DECELERATION_FACTOR * 3.0f * deltaTime;
                else if (ballSpeed < 0) ballSpeed += DECELERATION_ROULETTE * BALL_DECELERATION_FACTOR * 3.0f * deltaTime;

                float targetDist = ROULETTE_WHEEL_RADIUS * 0.65f;
                ballDistanceFromCenter = ballDistanceFromCenter + (targetDist - ballDistanceFromCenter) * deltaTime * 5.0f;

                if (fabsf(rouletteSpeed) < 5.0f && fabsf(ballSpeed) < 5.0f) {
                    rouletteSpeed = 0;
                    ballSpeed = 0;

                    float finalBallAngleRelativeToWheel = ballAngle - rouletteAngle;
                    while (finalBallAngleRelativeToWheel < 0) finalBallAngleRelativeToWheel += 360.0f;
                    finalBallAngleRelativeToWheel = fmodf(finalBallAngleRelativeToWheel, 360.0f);
                    float anglePerSlot = 360.0f / NUM_ROULETTE_NUMBERS;
                    int winningSlotIndex = (int)roundf(finalBallAngleRelativeToWheel / anglePerSlot - 0.4f) % NUM_ROULETTE_NUMBERS;
                    winningNumber = rouletteNumbers[winningSlotIndex];

                    float winningAngle = winningSlotIndex * anglePerSlot + anglePerSlot / 2.0f;
                    ballAngle = rouletteAngle + winningAngle;

                    // StopSound(spinSoundRoulette);
                    // StopSound(ballClickSound);

                    CalculateRouletteWinnings();
                    if (winningsThisRound > 0) {
                       // PlaySound(winSoundRoulette);
                       money += winningsThisRound;
                    } else if (totalBetThisRound > 0){
                       // PlaySound(loseSoundRoulette);
                    }

                    currentRouletteState = ROULETTE_STATE_RESULT;
                    rouletteTimer = 0.0f;
                }
             }
            break;

        case ROULETTE_STATE_RESULT:
            {
                rouletteTimer += deltaTime;

                DrawRouletteWheel((Vector2){ ROULETTE_CENTER_X, ROULETTE_CENTER_Y }, ROULETTE_WHEEL_RADIUS, rouletteAngle);
                Vector2 ballPos = { ROULETTE_CENTER_X + cosf(ballAngle * DEG2RAD) * ballDistanceFromCenter, ROULETTE_CENTER_Y + sinf(ballAngle * DEG2RAD) * ballDistanceFromCenter };
                DrawCircleV(ballPos, ROULETTE_BALL_RADIUS, ROULETTE_GOLD);

                const char* resultText;
                Color resultColor = WHITE;
                Color winnerColorDisplay = GetRouletteNumberColor(winningNumber);
                const char* winnerColorName = (winnerColorDisplay.r == ROULETTE_RED.r) ? "RED" : ((winnerColorDisplay.g == ROULETTE_GREEN.g) ? "GREEN" : "BLACK");

                if (winningsThisRound > 0) {
                    resultText = TextFormat(" %s\n You won:\n %s", winnerColorName,  FormatMoney(winningsThisRound - totalBetThisRound));
                    resultColor = GREEN;
                } else {
                    resultText = TextFormat(" %s\n You lost:\n %s", winnerColorName, FormatMoney(totalBetThisRound));
                    resultColor = RED;
                }

                DrawTextCentered(font36, resultText, (Rectangle){ ROULETTE_CENTER_X + ROULETTE_WHEEL_RADIUS, ROULETTE_CENTER_Y, 200, 0 }, 36, resultColor);

                if (rouletteTimer >= RESULT_DISPLAY_TIME) {
                    ResetRouletteRound();
                    playerBet = 0;
                    currentRouletteState = ROULETTE_STATE_BETTING;
                }
            }
            return;
            break;
    }

    DrawRouletteWheel((Vector2){ ROULETTE_CENTER_X, ROULETTE_CENTER_Y }, ROULETTE_WHEEL_RADIUS, rouletteAngle);

    Vector2 ballPos = {
        ROULETTE_CENTER_X + cosf(ballAngle * DEG2RAD) * ballDistanceFromCenter,
        ROULETTE_CENTER_Y + sinf(ballAngle * DEG2RAD) * ballDistanceFromCenter
    };
    DrawCircleV(ballPos, ROULETTE_BALL_RADIUS, ROULETTE_GOLD);

    if (currentRouletteState == ROULETTE_STATE_BETTING) {
        DrawTextEx(font36, TextFormat("Bet: %s", FormatMoney(playerBet)), (Vector2){995, 320}, 36, 2, WHITE);
    } else if (currentRouletteState == ROULETTE_STATE_SPINNING || currentRouletteState == ROULETTE_STATE_BALL_LANDING) {
        const char* betText = "";
         if(betOnRed > 0) { betText = TextFormat("Bet: %s\non RED", FormatMoney(betOnRed)); }
         else if(betOnBlack > 0) { betText = TextFormat("Bet: %s\non BLACK", FormatMoney(betOnBlack)); }
         else if(betOnGreen > 0) { betText = TextFormat("Bet: %s\non GREEN", FormatMoney(betOnGreen)); }
         DrawTextCentered(font36, betText, (Rectangle){ ROULETTE_CENTER_X + ROULETTE_WHEEL_RADIUS, ROULETTE_CENTER_Y, 200, 0 }, 36, WHITE);
    }
}

Color GetRouletteNumberColor(int number) {
    if (number == 0) return ROULETTE_GREEN;
    switch (number) {
        case 1: case 3: case 5: case 7: case 9: case 12: case 14: case 16:
        case 18: case 19: case 21: case 23: case 25: case 27: case 30:
        case 32: case 34: case 36:
            return ROULETTE_RED;
        default:
            return ROULETTE_BLACK;
    }
}

void DrawRouletteWheel(Vector2 center, float radius, float rotation) {
    float anglePerSlot = 360.0f / NUM_ROULETTE_NUMBERS;

    DrawCircleV(center, radius * 1.06f, (Color){100, 25, 0, 255});

    DrawRing(center, radius * 0.3f - 2, radius * 0.3f + 2, 0, 360, 16, DARKBROWN);
    DrawCircleV(center, radius * 0.3f, (Color){91,73,31, 255});

    for (int i = 0; i < NUM_ROULETTE_NUMBERS; i++) {
        int currentNumber = rouletteNumbers[i];
        Color segmentColor = GetRouletteNumberColor(currentNumber);
        float startAngle = rotation + i * anglePerSlot;
        float endAngle = startAngle + anglePerSlot;

        DrawRing(center, radius * 0.3f, radius, startAngle, endAngle, 16, segmentColor);

        float textAngle = startAngle + anglePerSlot / 2.0f;
        float textRadius = radius * 0.9f;
        Vector2 textPos = { center.x + cosf(textAngle * DEG2RAD) * textRadius, center.y + sinf(textAngle * DEG2RAD) * textRadius };
        // Note: Assuming font36 is a typo and you meant font16 based on DrawTextEx usage
        Vector2 textSize = MeasureTextEx(font16, TextFormat("%d", currentNumber), 16, 0);
        textPos.x -= textSize.x / 2.0f;
        textPos.y -= textSize.y / 2.0f;
        DrawTextEx(font16, TextFormat("%d", currentNumber), (Vector2){textPos.x, textPos.y}, 16, 0, WHITE);

        Vector2 lineEnd = { center.x + cosf(startAngle * DEG2RAD) * radius, center.y + sinf(startAngle * DEG2RAD) * radius };
        Vector2 lineStart = { center.x + cosf(startAngle * DEG2RAD) * radius * 0.3f, center.y + sinf(startAngle * DEG2RAD) * radius * 0.3f };
        DrawLineEx(lineStart, lineEnd, 1.0f, Fade(GRAY, 0.5f));
    }

    Color crossColor = GOLD;
    float crossThickness = 3.0f;
    float crossLength = radius * 0.5f;

    float angle1_start = (rotation + 0) * DEG2RAD;
    float angle1_end = (rotation + 180) * DEG2RAD;

    Vector2 line1_start = { center.x + cosf(angle1_start) * crossLength / 2.0f, center.y + sinf(angle1_start) * crossLength / 2.0f };
    Vector2 line1_end = { center.x + cosf(angle1_end) * crossLength / 2.0f, center.y + sinf(angle1_end) * crossLength / 2.0f };

    DrawLineEx(line1_start, line1_end, crossThickness, crossColor);

    float angle2_start = (rotation + 90) * DEG2RAD;
    float angle2_end = (rotation + 270) * DEG2RAD;

    Vector2 line2_start = { center.x + cosf(angle2_start) * crossLength / 2.0f, center.y + sinf(angle2_start) * crossLength / 2.0f };
    Vector2 line2_end = { center.x + cosf(angle2_end) * crossLength / 2.0f, center.y + sinf(angle2_end) * crossLength / 2.0f };

    DrawLineEx(line2_start, line2_end, crossThickness, crossColor);

    DrawCircleV(center, radius * 0.05f, crossColor);
}

void CalculateRouletteWinnings() {
    winningsThisRound = 0;
    if (winningNumber < 0 || winningNumber >= NUM_ROULETTE_NUMBERS) return;

    Color winnerColor = GetRouletteNumberColor(winningNumber);

    if (winnerColor.r == ROULETTE_RED.r && winnerColor.g == ROULETTE_RED.g && betOnRed > 0) {
        winningsThisRound = betOnRed * 2;
    }

    else if (winnerColor.r == ROULETTE_BLACK.r && winnerColor.g == ROULETTE_BLACK.g && betOnBlack > 0) {
        winningsThisRound = betOnBlack * 2;
    }

    else if (winnerColor.r == ROULETTE_GREEN.r && winnerColor.g == ROULETTE_GREEN.g && betOnGreen > 0) {
        winningsThisRound = betOnGreen * 36;
    }
}

void ResetRouletteRound() {
    betOnRed = 0;
    betOnBlack = 0;
    betOnGreen = 0;
    totalBetThisRound = 0;
    winningsThisRound = 0;
}
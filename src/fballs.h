#define NUM_ROWS 6
#define NUM_COLS 15
#define BALL_RADIUS 8
#define PIN_RADIUS 6

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool active;
    int reward;
} fBall;

Vector2 pins[NUM_ROWS * NUM_COLS];
int pinCount = 0;

fBall fBalls[MAX_BALLS] = { 0 };

int fBallCount = 0;
int remainingBalls = 0;

int playingBalls = 0;
int allBallsInactive = 0;
int reward = 0;
float displayedReward = 0.0f;

int rewards[] = { 500, 100, 25, 100, 500 };

void HandleBallPinCollision(Vector2 pin, fBall *ball) {
    Vector2 collisionVector = Vector2Subtract(ball->position, pin);
    float distance = Vector2Length(collisionVector);

    if (distance < BALL_RADIUS + PIN_RADIUS) {
        Vector2 collisionNormal = Vector2Scale(collisionVector, 1.0f / distance);
        float velocityAlongNormal = Vector2DotProduct(ball->velocity, collisionNormal);
        Vector2 reflectedVelocity = Vector2Subtract(ball->velocity, Vector2Scale(collisionNormal, 1.5f * velocityAlongNormal));
        ball->velocity = reflectedVelocity;
        float overlap = (BALL_RADIUS + PIN_RADIUS) - distance;
        ball->position = Vector2Add(ball->position, Vector2Scale(collisionNormal, overlap));
    }
}

void fBallInit() {
    float startX = 700 / 2.0f;  // Updated for new width
    float startY = 80.0f;       // Adjusted to fit in shorter height
    float spacingX = 35.0f;     // Slightly reduced spacing to fit
    float spacingY = 35.0f;     // Slightly reduced spacing to fit

    for (int row = 0; row < NUM_ROWS; row++) {
        int numPinsInRow = row + 4;
        float rowStartX = startX - (numPinsInRow - 1) * spacingX / 2.0f;
        for (int col = 0; col < numPinsInRow; col++) {
            pins[pinCount++] = (Vector2){rowStartX + col * spacingX, startY + row * spacingY};
        }
    }
}

void DrawRemainingBalls() {
    float startX = 650.0f;    // Moved to the right for wider window
    float startY = 40.0f;     // Moved up slightly for shorter window
    float spacing = 20.0f;

    for (int i = 0; i < remainingBalls; i++) {
        DrawCircle(startX - i * spacing, startY, BALL_RADIUS, GRAY);
    }
}

void fBallUpdate(float deltaTime) {
    bonusTimer += deltaTime;
    // bonusTimer = 10000;
    if (!playingBalls && bonusTimer > 5 * 60 && Blue_Button((Rectangle){ 40, 40, 100, 40 }, TEXT_BONUS, (Color){160, 40, 150, 255})) {
        fBallCount = 0;
        reward = 0;
        playingBalls = 1;
        remainingBalls = 10;
        allBallsInactive = 0;
        displayedReward = 0.0f;
        bonusTimer = 0;

        int mult = (moneyMult + 1);
        rewards[0] = 500 * mult;
        rewards[1] = 250 * mult;
        rewards[2] = 100 * mult;
        rewards[3] = 250 * mult;
        rewards[4] = 500 * mult;
    }

    if (playingBalls) {
        DrawRectangleRounded((Rectangle){ 25, 25, 650, 370 }, 0.05f, 16, (Color){ 0, 0, 0, 216 });  // Updated for new dimensions
        DrawRemainingBalls();

        if (Blue_Button((Rectangle){ 350 - 75, 335, 150, 40 }, "Play", (Color){160, 40, 150, 255})) {  // Updated position and size
            if (remainingBalls > 0) {
                fBalls[fBallCount++] = (fBall){(Vector2){700 / 2 + GetRandomValue(-50, 50), 50}, (Vector2){GetRandomValue(-1000, 1000) / 500.0f, 0}, true, -1};  // Updated for new width
                remainingBalls--;
            }
        }

        if (displayedReward < reward) {
            displayedReward += (reward - displayedReward) * 20.0f * deltaTime;
            if (displayedReward > reward) {
                displayedReward = reward;
            }
        }

        DrawTextEx(font36, FormatMoney(ceil(displayedReward)), (Vector2){60, 50}, 36, 0, GREEN);

        allBallsInactive = 1;
        for (int i = 0; i < fBallCount; i++) {
            if (!fBalls[i].active) continue;
            allBallsInactive = 0;

            fBalls[i].velocity.y += 400.0f * deltaTime;
            fBalls[i].position = Vector2Add(fBalls[i].position, Vector2Scale(fBalls[i].velocity, deltaTime));

            DrawCircleV(fBalls[i].position, BALL_RADIUS, MAROON);

            for (int j = 0; j < pinCount; j++) {
                HandleBallPinCollision(pins[j], &fBalls[i]);
            }

            if (fBalls[i].position.y > 420) {  // Updated for new height
                fBalls[i].active = false;
            }

            if (fBalls[i].position.x > 660) {  // Updated for new width
                fBalls[i].position.x = 660;
                fBalls[i].velocity.x *= -1;
            }

            if (fBalls[i].position.x < 40) {
                fBalls[i].position.x = 40;
                fBalls[i].velocity.x *= -1;
            }

            // Updated reward boxes for new width
            for (int j = 0; j < 5; j++) {
                Rectangle rewardBox = (Rectangle){50 + j * 120, 290, 120, 35};  // Adjusted for new dimensions

                if ((fBalls[i].active && CheckCollisionCircleRec(fBalls[i].position, BALL_RADIUS, rewardBox)) || fBalls[i].position.y > 420) {  // Updated height
                    fBalls[i].active = false;
                    fBalls[i].reward = rewards[j];
                    reward += fBalls[i].reward;
                }
            }
        }

        for (int i = 0; i < pinCount; i++) {
            DrawCircleV(pins[i], PIN_RADIUS, (Color){139, 105, 20, 255});
        }

        // Updated reward boxes for drawing
        for (int i = 0; i < 5; i++) {
            Rectangle rewardBox = (Rectangle){50 + i * 120, 290, 120, 35};  // Adjusted for new dimensions
            DrawRectangleRec(rewardBox, i % 2 ? (Color){64, 170, 170, 255} : (Color){90, 190, 190, 255});
            DrawTextCentered(font24, FormatMoney(rewards[i]), rewardBox, 24, BLACK);
        }

        if (remainingBalls == 0 && allBallsInactive) {
            if (Blue_Button((Rectangle){ 515, 335, 150, 40 }, TEXT_CONTINUE, BUTTON_COLOR)) {  // Updated position and size
                playingBalls = 0;
                money += reward;
            }
        }
    }
}
float slots[3] = {0};
float slotsSpeeds[3] = {0};
bool spinning = false;
int hasWon = 0;
float spinTime = 0.0f;
float spinDuration = 3.0f;

float machineTimer = 0;

void UpdateSlot(float deltaTime) {
    const char* betText = TextFormat("Bet: %s", FormatMoney(playerBet));

    if(!spinning && !lockBet && playerBet > 0){
        if(Blue_Button((Rectangle){ 1130, 320, 50, 50 }, "", BUTTON_COLOR)) {
            lockBet = true;
        }

        DrawTexturePro(
            lockTexture,
            (Rectangle){0, 0, lockTexture.width, lockTexture.height},
            (Rectangle){1130 + 12, 320 + 7, 30, 30},
            (Vector2){0, 0},
            0,
            WHITE
        );
    }

    DrawRectangle(820, 160, 80 + 2 * 90, 80, (Color){37, 37, 37, 255});

    for (int i = 0; i < 3; i++) {
        Rectangle bounds = (Rectangle){820 + i * 90, 160, 80, 80};
        DrawRectangleRounded(bounds, 0.05f, 32, (Color){ 255, 255, 255, 255 });
        DrawTexturePro(slotSpriteSheet, (Rectangle){0, slots[i] * 64, 64, 64}, bounds, (Vector2){ 0, 0 }, 0, WHITE);
    }

    int currentFrame = 0;

    if(machineTimer < 0.3f) {
        currentFrame = machineTimer / 0.1f;
    } else if (machineTimer < 0.6f) {
        currentFrame = 2 - (int)((machineTimer - 0.3f) / 0.1f);
    }

    DrawTexturePro(slotMachineTexture, (Rectangle){0, 190 * currentFrame, 374, 190}, (Rectangle){790, 110, 374, 190}, (Vector2){ 0, 0 }, 0, WHITE);

    if (Blue_Button((Rectangle){860, 310, 180, 55}, "SPIN", BUTTON_COLOR)) {
        if (!spinning && playerBet > 0) {
            spinning = true;
            spinTime = 0.0f;
            hasWon = 0;
            machineTimer = 0;
            for (int i = 0; i < 3; i++) {
                slotsSpeeds[i] = (float)GetRandomValue(8, 15) + i * 3;
            }
            PlaySound(spinStartEffect);
        }
    }


    DrawTextCentered(font36, betText, (Rectangle){800, 240, 300, 60}, 36, WHITE);

    if(hasWon > 0) {
        DrawTextCentered(font36, TextFormat("Won: %s", FormatMoney(hasWon)), (Rectangle){800, 105, 300, 60}, 36, GREEN);
    } else if(hasWon < 0) {
        DrawTextCentered(font36, TextFormat("Lost: %s", FormatMoney(hasWon * -1)), (Rectangle){800, 105, 300, 60}, 36, RED);
    } else {
        DrawTextCentered(font36, "Slots", (Rectangle){800, 105, 300, 60}, 36, WHITE);
    }

    if (spinning) {
        machineTimer += deltaTime;

        spinTime += deltaTime;

        for (int i = 0; i < 3; i++) {
            if(slotsSpeeds[i] > 0.05f) {
                slotsSpeeds[i] -= deltaTime * 7;
            } else {
                float target = (float)(((int)roundf(slots[i])));
                slots[i] = slots[i] + (target - slots[i]) * deltaTime * 6.0f;
                slotsSpeeds[i] = 0;
            }
        }

        for (int i = 0; i < 3; i++) {
            slots[i] += slotsSpeeds[i] * deltaTime;
        }

        if (spinTime >= spinDuration) {
            StopSound(spinStartEffect);
            spinning = false;
            PlaySound(spinEndEffect);

            int mult = 1;
            int cherry = 0;

            int s1 = (int)round(slots[0]) % 8;
            int s2 = (int)round(slots[1]) % 8;
            int s3 = (int)round(slots[2]) % 8;

            slots[0] = (float)s1;
            slots[1] = (float)s2;
            slots[2] = (float)s3;

            if(s1 == 2) cherry++;
            if(s2 == 2) cherry++;
            if(s3 == 2) cherry++;
            if(cherry == 1) mult *= 2;
            if(cherry == 2) mult *= 3;

            if(s1 == s2 && s2 == s3) {
                mult *= 20;
            }

            if(mult > 1) {
                hasWon = playerBet * mult;
                money += playerBet * mult;
            } else {
                hasWon = -playerBet;
            }

            if(lockBet == true && money > playerBet) {
                money -= playerBet;
            } else {
                lockBet = false;
                playerBet = 0;
            }
        }
    }
}
#define MAX_CARDS 52
#define MAX_HAND_SIZE 12

typedef struct {
    int rank;
    int suit;
    Vector2 pos;
} Card;

enum BlackJackState {
  Main,
  PlayersTurn,
  DealersTurn,
  Ended
};

enum GameState {
  Win,
  Lose,
  Draw
};

Card deck[MAX_CARDS];

Card playerHand[MAX_HAND_SIZE];
Card dealerHand[MAX_HAND_SIZE];

int playerHandSize = 0;
int dealerHandSize = 0;
int playerScore = 0;
int dealerScore = 0;
int oldBet = 0;
int playerBet = 0;
int deckIndex = 0;
int isBlackJack = 0;
int canSwapHands = 0;
enum BlackJackState bjState = Main;
enum GameState bjGameState = Draw;



void InitDeck() {
    int index = 0;
    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 0; rank < 13; rank++) {
            deck[index].rank = rank;
            deck[index].suit = suit;
            index++;
        }
    }
}

void ShuffleDeck() {
    for (int i = 0; i < MAX_CARDS; i++) {
        int j = GetRandomValue(0, MAX_CARDS - 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}
// 0 player // 1 dealer
Card DrawCard(int who, int offset) {
    if (deckIndex < MAX_CARDS) {
        if(who == 0)
            deck[deckIndex].pos = (Vector2){1200 + offset, 300};
        if(who == 1)
            deck[deckIndex].pos = (Vector2){1200 + offset, 140};
        return deck[deckIndex++];
    } else {
        InitDeck();
        ShuffleDeck();
        deckIndex = 0;
        return deck[deckIndex++];
    }
}


int CalculateScore(Card hand[], int handSize) {
    int score = 0;
    int numAces = 0;

    for (int i = 0; i < handSize; i++) {
        int rank = hand[i].rank;
        if (rank > 0 && rank < 10) {
            score += rank + 1;
        } else if (rank >= 10) {
            score += 10;
        } else if (rank == 0) {
            score += 11;
            numAces++;
        }
    }

    while (score > 21 && numAces > 0) {
        score -= 10;
        numAces--;
    }

    return score;
}


void ShowCard(Texture2D texture, Vector2 position, float rotation, int rank, int suit){
    Rectangle frameRec = { rank * 64 + 12, suit * 64 + 2, 40, 60 };
    Rectangle destRec = { position.x + 40, position.y + 60, 80, 120 };
    DrawTexturePro(texture, frameRec, destRec, (Vector2){40, 60}, rotation, WHITE);
}

void ResetBlackJack() {
    ShuffleDeck();

    playerHandSize = 0;
    dealerHandSize = 0;
    playerScore = 0;
    dealerScore = 0;
    deckIndex = 0;
    isBlackJack = 0;
    oldBet = playerBet;
    canSwapHands = 0;

    if(GetRandomValue(1, 2) == 1) {
        canSwapHands = 1;
    }

    playerHand[0] = DrawCard(0, 120);
    dealerHand[0] = DrawCard(1, 240);
    playerHand[1] = DrawCard(0, 360);
    dealerHand[1] = DrawCard(1, 480);

    playerHandSize = 2;
    dealerHandSize = 2;

    playerScore = CalculateScore(playerHand, playerHandSize);
    dealerScore = CalculateScore(dealerHand, dealerHandSize);

    bjState = PlayersTurn;
    bjGameState = Draw;

    if (playerScore == 21) {
        if(dealerScore == 21) {
            money += playerBet;
            if(lockBet == true && money > playerBet) {
                money -= playerBet;
            } else {
                lockBet = false;
                playerBet = 0;
            }
            bjGameState = Draw;
            bjState = Ended;
        } else {
            oldBet = playerBet * 3;
            money += playerBet * 3;

            if(false) {
                money += playerBet * 7;
                oldBet = playerBet * 10;
            }

            if(lockBet == true && money > playerBet) {
                money -= playerBet;
            } else {
                lockBet = false;
                playerBet = 0;
            }
            bjGameState = Win;
            bjState = Ended;
        }
    }
}

void InitBlackJack() {
    InitDeck();
}

typedef struct {
    int isDragging;
    Vector2 originalPos;
    Vector2 currentPos;
    Vector2 mouseOffset;
    Vector2 velocity;
} CardDragState;

CardDragState cardStates[3] = {
    (CardDragState) {
        false,
        (Vector2){820 - 7 + 0 * 90, 110},
        (Vector2){820 - 7 + 0 * 90, 110},
        (Vector2){0, 0},
        (Vector2){0, 0}
    }, (CardDragState) {
        false,
        (Vector2){820 - 7 + 1 * 90, 110},
        (Vector2){820 - 7 + 1 * 90, 110},
        (Vector2){0, 0},
        (Vector2){0, 0}
    }, (CardDragState) {
        false,
        (Vector2){820 - 7 + 2 * 90, 110},
        (Vector2){820 - 7 + 2 * 90, 110},
        (Vector2){0, 0},
        (Vector2){0, 0}
    },

};

void UpdateMainMenuCardDragging(float deltaTime) {
    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < 3; i++) {
        Rectangle cardRect = {
            820 - 7 + i * 90, 110, 80, 120
        };

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            CheckCollisionPointRec(mousePos, cardRect)) {
            cardStates[i].isDragging = 1;
            cardStates[i].originalPos = (Vector2){820 - 7 + i * 90, 110};
            cardStates[i].mouseOffset = Vector2Subtract(mousePos, cardStates[i].originalPos);
            cardStates[i].velocity = (Vector2){0, 0};
        }

        if (cardStates[i].isDragging) {
            Vector2 targetPos = Vector2Subtract(mousePos, cardStates[i].mouseOffset);
            cardStates[i].currentPos = targetPos;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && cardStates[i].isDragging) {
            cardStates[i].isDragging = 0;
        }

        if (!cardStates[i].isDragging) {
            cardStates[i].currentPos = Vector2Add(
                cardStates[i].currentPos,
                Vector2Scale(cardStates[i].velocity, deltaTime)
            );

            cardStates[i].velocity = Vector2Add(cardStates[i].velocity, Vector2Scale(Vector2Subtract(cardStates[i].originalPos, cardStates[i].currentPos), deltaTime * 300));
            cardStates[i].velocity = Vector2Scale(cardStates[i].velocity, 0.89f);
        }
    }
}

void DrawMainMenuCards(Texture2D deckTexture) {
    for (int i = 0; i < 3; i++) {
        Vector2 drawPos = cardStates[i].isDragging ?
            cardStates[i].currentPos :
            (cardStates[i].currentPos.x != 0 ?
                cardStates[i].currentPos :
                (Vector2){820 - 7 + i * 90, 110});

        ShowCard(deckTexture, drawPos, Clamp(cardStates[i].velocity.x * 0.01f, -25, 25), 12 - i, i);
    }
}

// Initialize in your setup function
void InitCardDragging() {
    for (int i = 0; i < 3; i++) {
        cardStates[i] = (CardDragState){0};
    }
}

void UpdateBlackJack(float deltaTime) {
    const char* betText = TextFormat(TEXT_BET, FormatMoney(playerBet));

    if(bjState == Main) {
        if (Blue_Button((Rectangle){ 820, 250, 250, 59 }, "BlackJack", BUTTON_COLOR) && playerBet > 0) {
            ResetBlackJack();
            PlaySound(take4Effect);
        }

        DrawTextCentered(font36, betText, (Rectangle){820, 340, 250, 0}, 36, WHITE);
        if(!lockBet && playerBet > 0){
            if(Blue_Button((Rectangle){ 1130, 320, 50, 50 }, "", BUTTON_COLOR)) {
                lockBet = true;
            }

            DrawTexturePro(
                lockTexture,
                (Rectangle){0, 0, lockTexture.width, lockTexture.height},
                (Rectangle){1130 + 12, 320 + 7, 30, 30 },
                (Vector2){0, 0},
                0,
                WHITE
            );
        }


        UpdateMainMenuCardDragging(deltaTime);
        DrawMainMenuCards(deckTexture);
    }

    if(bjState == Ended) {
        if (Blue_Button((Rectangle){ 850, 460, 200, 60 }, TEXT_AGAIN, BUTTON_COLOR)) {
            bjState = Main;
        }
    }

    if (bjState == PlayersTurn) {
        if (Blue_Button((Rectangle){ 745, 460, 200, 60 }, TEXT_HIT, BUTTON_COLOR)) {
            playerHand[playerHandSize] = DrawCard(0, 100);
            playerHandSize++;

            playerScore = CalculateScore(playerHand, playerHandSize);

            if (playerScore >= 21) {
                bjState = DealersTurn;
            }

            PlaySound(take1Effect);

            if(save.vibration){
                Vibrate(0.25f);
            }
        }

        if (Blue_Button((Rectangle){ 955, 460, 200, 60 }, TEXT_STAND, BUTTON_COLOR)) {
            bjState = DealersTurn;
            if(save.vibration){
                Vibrate(0.25f);
            }
        }

        if (swapHands && canSwapHands  && playerHandSize == 2 && dealerHandSize == 2 && Blue_Button((Rectangle){ 1050, 250, 120, 40 }, TEXT_SWAP_BTN, BUTTON_COLOR)) {
            Card tempCard = playerHand[0];
            playerHand[0] = dealerHand[1];
            dealerHand[1] = tempCard;

            tempCard = playerHand[1];
            playerHand[1] = dealerHand[0];
            dealerHand[0] = tempCard;

            playerScore = CalculateScore(playerHand, playerHandSize);
            dealerScore = CalculateScore(dealerHand, dealerHandSize);
        }
    }

    if(bjState == DealersTurn) {
        int i = 0;
        while(playerScore < 22 && dealerScore < 17) {
            dealerHand[dealerHandSize] = DrawCard(1, 100 + i);
            dealerHandSize++;
            dealerScore = CalculateScore(dealerHand, dealerHandSize);
            PlaySound(take1Effect);
            i += 100;
        }

        if ((dealerScore < playerScore || 21 < dealerScore) && playerScore < 22) {
            oldBet = playerBet * 2;
            money += playerBet * 2;
            bjGameState = Win;
        } else if(dealerScore == playerScore) {
            money += playerBet;
            bjGameState = Draw;
        } else {
            bjGameState = Lose;

            if(retainLoss) {
                money += playerBet / 4;
                oldBet = (playerBet * 3) / 4;
            }
        }

        if(lockBet == true && money > playerBet) {
            money -= playerBet;
        } else {
            lockBet = false;
            playerBet = 0;
        }
        bjState = Ended;
    }

    if(bjState != Main) {
        if(bjState != Ended) {
            DrawTextEx(font48, betText, (Vector2){775, 105}, 48, 2, WHITE);
        } else {
              switch (bjGameState) {
                case Win:
                    DrawTextEx(font48, TextFormat("+%s", FormatMoney(oldBet)), (Vector2){980, 103}, 48, 2, GREEN);
                    DrawTextEx(font48, TEXT_WIN, (Vector2){775, 103}, 48, 2, GREEN);
                  break;
                case Lose:
                    DrawTextEx(font48, TextFormat("-%s", FormatMoney(oldBet)), (Vector2){1000, 103}, 48, 2, RED);
                    DrawTextEx(font48, TEXT_LOSE, (Vector2){775, 103}, 48, 2, RED);
                  break;
                case Draw:
                    DrawTextEx(font48, TextFormat("+%s", FormatMoney(oldBet)), (Vector2){1000, 103}, 48, 2, WHITE);
                    DrawTextEx(font48, TEXT_DRAW, (Vector2){775, 103}, 48, 2, WHITE);
                  break;
              }
        }

        if(bjState == Ended || seeFirstCard) {
            DrawTextEx(font48, TextFormat("%d", dealerScore), (Vector2){770, 190}, 48, 2, WHITE);
            for (int i = 0; i < dealerHandSize; i++) {
                const Vector2 dir = Vector2Subtract((Vector2){825 + i * 40, 160}, dealerHand[i].pos);
                const float length = Vector2Length(dir);
                dealerHand[i].pos = Vector2Add(dealerHand[i].pos, Vector2Scale(Vector2Normalize(dir) , MIN(1000.0f * deltaTime, length)));

                ShowCard(deckTexture, dealerHand[i].pos, MIN(-dir.x / 6, 20.0f), dealerHand[i].rank, dealerHand[i].suit);
            }
        } else {
            const Vector2 dir = Vector2Subtract((Vector2){825, 160}, dealerHand[0].pos);
            const float length = Vector2Length(dir);
            dealerHand[0].pos = Vector2Add(dealerHand[0].pos, Vector2Scale(Vector2Normalize(dir) , MIN(1000.0f * deltaTime, length)));

            DrawTexturePro(
                backTexture,
                (Rectangle){0, 0, backTexture.width, backTexture.height},
                (Rectangle){dealerHand[0].pos.x, dealerHand[0].pos.y, 80, 120},
                (Vector2){0, 0},
                MIN(-dir.x / 6, 20.0f),
                WHITE
            );

            for (int i = 1; i < dealerHandSize; i++) {
                const Vector2 dir = Vector2Subtract((Vector2){825 + i * 40, 160}, dealerHand[i].pos);
                const float length = Vector2Length(dir);
                dealerHand[i].pos = Vector2Add(dealerHand[i].pos, Vector2Scale(Vector2Normalize(dir) , MIN(1000.0f * deltaTime, length)));

                ShowCard(deckTexture, dealerHand[i].pos, MIN(-dir.x / 6, 20.0f), dealerHand[i].rank, dealerHand[i].suit);
            }
        }

        DrawTextEx(font48, TextFormat("%d", playerScore), (Vector2){770, 340}, 48, 2, WHITE);
        for (int i = 0; i < playerHandSize; i++) {

            const Vector2 dir = Vector2Subtract((Vector2){825 + i * 40, 310}, playerHand[i].pos);
            const float length = Vector2Length(dir);
            playerHand[i].pos = Vector2Add(playerHand[i].pos, Vector2Scale(Vector2Normalize(dir) , MIN(1000.0f * deltaTime, length)));
            ShowCard(deckTexture, playerHand[i].pos, MIN(-dir.x / 6, 20.0f), playerHand[i].rank, playerHand[i].suit);
        }
    }
}


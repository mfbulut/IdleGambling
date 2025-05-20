#include <stddef.h>
#include <string.h>

#define MIN(a, b)(((a) < (b)) ? (a) : (b))
#define MAX(a, b)(((a) > (b)) ? (a) : (b))

void DrawRectangleShadowed(Rectangle rect, float rounding, Color color, Color shadowColor, Vector2 shadowOffset) {
    DrawRectangleRounded((Rectangle){rect.x + shadowOffset.x, rect.y + shadowOffset.y, rect.width, rect.height}, rounding, 16, shadowColor);
    DrawRectangleRounded(rect, rounding, 16, color);
}

void DrawTextCentered(Font font, const char* text, Rectangle bounds, int fontSize, Color color) {
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 0);
    Vector2 textPosition = { bounds.x + (bounds.width / 2) - (textSize.x / 2),
                             bounds.y + (bounds.height / 2) - (textSize.y / 2) };
    DrawTextEx(font, text, textPosition, fontSize, 0, color);
}
/*
bool Button(Rectangle rect, char* text, Color buttonColor) {
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), rect);
    bool isPressed = isHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool isClicked = isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    Color primaryColor =  isPressed ? ColorBrightness(buttonColor, -0.1f) :
                          (isHovered ? ColorBrightness(buttonColor, 0.1f) : buttonColor);

    if(isPressed) {
        Rectangle new = (Rectangle){rect.x, rect.y + 3, rect.width, rect.height};
        DrawRectangleShadowed(new, 0.25f, primaryColor, ColorBrightness(buttonColor, -0.4f), (Vector2){ 0, 2 });
        if(new.height <= 50)
            DrawTextCentered(font24, text, new, 24, WHITE);
        else if(new.height < 80)
            DrawTextCentered(font36, text, new, 36, WHITE);
        else
            DrawTextCentered(font48, text, new, 48, WHITE);
    } else {
        DrawRectangleShadowed(rect, 0.25f, primaryColor, ColorBrightness(buttonColor, -0.4f), (Vector2) {0,5});
        if(rect.height <= 50)
            DrawTextCentered(font24, text, rect, 24, WHITE);
        else if(rect.height < 80)
            DrawTextCentered(font36, text, rect, 36, WHITE);
        else
            DrawTextCentered(font48, text, rect, 48, WHITE);
    }
    return isClicked;
}
*/

bool Red_Button(Rectangle rect, char* text, Color buttonColor) {
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), rect);
    bool isPressed = isHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool isClicked = isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    Color primaryColor =  isPressed ? ColorBrightness(WHITE, -0.2f) : (isHovered ? ColorBrightness(WHITE, -0.1f) : WHITE);

    NPatchInfo nPatchInfo = {(Rectangle) {0, 0, redButtonTexture.width, redButtonTexture.height}, 16, 16, 16, 16, NPATCH_NINE_PATCH};
    DrawTextureNPatch(redButtonTexture, nPatchInfo, rect, (Vector2){0, 0}, 0.0, primaryColor);

    Rectangle new = (Rectangle){rect.x, rect.y - 4, rect.width, rect.height};

    if(new.height <= 50)
        DrawTextCentered(font24, text, new, 24, WHITE);
    else if(new.height < 80)
        DrawTextCentered(font36, text, new, 36, WHITE);
    else
        DrawTextCentered(font48, text, new, 48, WHITE);

    return isClicked;
}

bool Blue_Button(Rectangle rect, char* text, Color buttonColor) {
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), rect);
    bool isPressed = isHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool isClicked = isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    Color primaryColor =  isPressed ? ColorBrightness(WHITE, -0.2f) : (isHovered ? ColorBrightness(WHITE, -0.1f) : WHITE);

    NPatchInfo nPatchInfo = {(Rectangle) {0, 0, blueButtonTexture.width, blueButtonTexture.height}, 16, 16, 16, 16, NPATCH_NINE_PATCH};
    DrawTextureNPatch(blueButtonTexture, nPatchInfo, rect, (Vector2){0, 0}, 0.0, primaryColor);

    Rectangle new = (Rectangle){rect.x, rect.y - 4, rect.width, rect.height};

    if(new.height <= 50)
        DrawTextCentered(font24, text, new, 24, WHITE);
    else if(new.height < 80)
        DrawTextCentered(font33, text, new, 33, WHITE);
    else
        DrawTextCentered(font48, text, new, 48, WHITE);

    return isClicked;
}

bool Green_Button(Rectangle rect, char* text, Color buttonColor) {
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), rect);
    bool isPressed = isHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool isClicked = isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    Color primaryColor =  isPressed ? ColorBrightness(WHITE, -0.2f) : (isHovered ? ColorBrightness(WHITE, -0.1f) : WHITE);

    NPatchInfo nPatchInfo = {(Rectangle) {0, 0, greenButtonTexture.width, greenButtonTexture.height}, 16, 16, 16, 16, NPATCH_NINE_PATCH};
    DrawTextureNPatch(greenButtonTexture, nPatchInfo, rect, (Vector2){0, 0}, 0.0, primaryColor);

    Rectangle new = (Rectangle){rect.x, rect.y - 4, rect.width, rect.height};

    if(new.height <= 50)
        DrawTextCentered(font24, text, new, 24, WHITE);
    else if(new.height < 80)
        DrawTextCentered(font36, text, new, 36, WHITE);
    else
        DrawTextCentered(font48, text, new, 48, WHITE);

    return isClicked;
}

bool Empty_Button(Rectangle rect) {
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), rect);
    bool isClicked = isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    return isClicked;
}

float VolumeSlider(Rectangle rect, float currentVolume) {
    Color sliderColor = GRAY;
    Color handleColor = WHITE;

    DrawRectangleRounded(rect, 1.0f, 16, sliderColor);

    float handleX = rect.x + (currentVolume * rect.width);
    Rectangle handle = {handleX - 5, rect.y - 5, 10, rect.height + 10};

    Rectangle collisionRect = {rect.x - 5, rect.y - 25, rect.width + 10, rect.height + 50};

    if (CheckCollisionPointRec(GetMousePosition(), collisionRect) &&
        IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

        float newVolume = (GetMouseX() - rect.x) / rect.width;
        currentVolume = Clamp(newVolume, 0.0f, 1.0f);
    }

    DrawRectangleRounded((Rectangle){rect.x, rect.y , currentVolume * rect.width , rect.height}, 1.0f, 16, (Color){230,230,230,255});


    DrawRectangleRounded(handle, 1.0f, 16, handleColor);

    return currentVolume;
}

bool VibrationToggle(Rectangle rect, bool currentState) {
    Color onColor = GREEN;
    Color offColor = DARKGRAY;

    DrawRectangleRounded(rect, 0.5f, 10, currentState ? onColor : offColor);

    Rectangle handle = {
        currentState ? rect.x + rect.width - rect.height : rect.x,
        rect.y,
        rect.height,
        rect.height
    };
    DrawRectangleRounded(handle, 0.5f, 10, WHITE);


    if (CheckCollisionPointRec(GetMousePosition(), rect) &&
        IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        return !currentState;
    }

    return currentState;
}




#if defined(PLATFORM_WEB)
// implement using emscripten_run_script("alert('hi')");

static void SaveState() {
    const unsigned char *data = (const unsigned char *)&save;
    int dataSize = sizeof(GameSave);

    // Create a base64 string from binary data
    EM_ASM({
        let data = new Uint8Array(HEAPU8.buffer, $0, $1);
        let b64 = btoa(String.fromCharCode.apply(null, data));
        localStorage.setItem('game_save', b64);
        console.log("Game saved to localStorage.");
    }, data, dataSize);
}

// Helper to read a JS string (Base64) back into binary data
static void LoadSave() {
    unsigned char *data = (unsigned char *)&save;

    EM_ASM({
        let b64 = localStorage.getItem('game_save');
        if (b64) {
            let str = atob(b64);
            let bytes = new Uint8Array(str.length);
            for (let i = 0; i < str.length; ++i) {
                bytes[i] = str.charCodeAt(i);
            }
            HEAPU8.set(bytes, $0);
            console.log("Game loaded from localStorage.");
        } else {
            console.log("No save found in localStorage.");
        }
    }, data);
}

#else

void SaveState() {
    SaveFileData("game.save", &save, sizeof(GameSave));
}

void LoadSave() {
    int dataSize = sizeof(GameSave);
    unsigned char *fileData = LoadFileData("game.save", &dataSize);

    if (fileData != 0 && dataSize == sizeof(GameSave)) {
        memcpy(&save, fileData, sizeof(GameSave));
    }

    UnloadFileData(fileData);
}

#endif


char formatted[64];
char* FormatMoney(int amount) {
    const char* buffer = TextFormat("%d", amount);

    int len = TextLength(buffer);
    int commas = (len - 1) / 3;
    int new_len = len + commas + 1;

    formatted[new_len] = '\0';
    int j = new_len - 1;

    for (int i = len - 1, comma_count = 0; i >= 0; i--, j--) {
        if (comma_count == 3) {
            formatted[j] = ',';
            comma_count = 0;
            j--;
        }
        formatted[j] = buffer[i];
        comma_count++;
    }

    formatted[0] = '$';

    return formatted;
}
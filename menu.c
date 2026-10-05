#include "menu.h"

static Texture2D introTexture = {0};
static Texture2D menuBackground = {0};

#define ABOUT_PHOTO_LEFT  "ankon.png"
#define ABOUT_PHOTO_RIGHT "sayon.png"

static const char *ResolveMenuAsset(const char *name)
{
    static char resolved[1024];

    if (FileExists(name))
        return name;

    TextCopy(
        resolved,
        TextFormat("%s%s", GetApplicationDirectory(), name)
    );

    if (FileExists(resolved))
        return resolved;

    return name;
}

static void DrawFullScreenTexture(Texture2D texture)
{
    if (texture.id != 0)
    {
        Rectangle source =
        {
            0,
            0,
            (float)texture.width,
            (float)texture.height
        };

        Rectangle destination =
        {
            0,
            0,
            (float)GetScreenWidth(),
            (float)GetScreenHeight()
        };

        DrawTexturePro(
            texture,
            source,
            destination,
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
    }
}

static void DrawButton(
    Rectangle button,
    const char *label,
    bool hovered,
    Color accent
)
{
    Color fill = hovered
        ? (Color){65, 75, 112, 240}
        : (Color){35, 40, 64, 230};

    int fontSize = 22;

    DrawRectangleRounded(
        button,
        0.16f,
        8,
        fill
    );

    DrawRectangleLinesEx(
        button,
        2,
        hovered ? accent : GRAY
    );

    DrawText(
        label,
        (int)(
            button.x +
            (button.width - MeasureText(label, fontSize)) / 2
        ),
        (int)(button.y + 18),
        fontSize,
        WHITE
    );
}

static void DrawHeader(void)
{
    Rectangle headerBox = {220, 65, 380, 130};

    DrawRectangleRounded(
        headerBox,
        0.15f,
        8,
        (Color){20, 24, 40, 220}
    );

    DrawRectangleRoundedLines(
        headerBox,
        0.15f,
        8,
        GOLD
    );

    DrawText(
        " GRIDLOCK7",
        245,
        80,
        48,
        RED
    );

    DrawText(
        "SOKOBAN",
        295,
        135,
        42,
        GOLD
    );
}

static void DrawSmallRecord(
    BestRecord record,
    int x,
    int y
)
{
    if (record.hasRecord)
    {
        DrawText(
            TextSubtext(record.name, 0, 14),
            x,
            y,
            15,
            WHITE
        );

        DrawText(
            TextFormat(
                "%02i:%05.2f",
                (int)(record.time / 60.0),
                record.time - ((int)(record.time / 60.0) * 60)
            ),
            x + 145,
            y,
            15,
            GOLD
        );
    }
    else
    {
        DrawText(
            "NO RECORD YET",
            x,
            y,
            15,
            GRAY
        );
    }
}

static MenuResult ShowHelp(void)
{
    Rectangle backButton = {350, 535, 130, 45};

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        bool backHover =
            CheckCollisionPointRec(mouse, backButton);

        BeginDrawing();

        ClearBackground((Color){10, 10, 18, 255});

        DrawHeader();

        DrawText(
            "PUSH EVERY CRATE ONTO A GLOWING GREEN GOAL.",
            95,
            220,
            18,
            WHITE
        );

        DrawText(
            "CRATES CAN BE PUSHED BUT NEVER PULLED.",
            95,
            255,
            18,
            GOLD
        );

        DrawText(
            "ONE BAD PUSH CAN COOK THE ENTIRE RUN.",
            95,
            290,
            18,
            RED
        );

        DrawText(
            "THERE IS NO UNDO.",
            95,
            325,
            18,
            WHITE
        );

        DrawText(
            "RESTART. THINK. LOCK IN.",
            95,
            360,
            18,
            GOLD
        );

        DrawText(
            "YOUR TIME IS RECORDED.",
            95,
            395,
            18,
            SKYBLUE
        );

        DrawText(
            "BEAT THE RECORD OR BECOME THE RECORD.",
            95,
            430,
            18,
            LIME
        );

        DrawText(
            "CONTROLS",
            95,
            470,
            18,
            SKYBLUE
        );

        DrawText(
            "WASD / ARROW KEYS : MOVE",
            95,
            495,
            16,
            WHITE
        );

        DrawText(
            "R : RESTART       ENTER : NEXT LEVEL",
            430,
            495,
            16,
            WHITE
        );

        DrawButton(
            backButton,
            "BACK",
            backHover,
            SKYBLUE
        );

        EndDrawing();

        if (
            backHover &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        )
        {
            return MENU_HELP;
        }

        if (
            IsKeyPressed(KEY_ENTER) ||
            IsKeyPressed(KEY_ESCAPE)
        )
        {
            return MENU_HELP;
        }
    }

    return MENU_EXIT;
}

static void DrawAboutPhoto(
    Texture2D texture,
    Rectangle frame,
    Color accent
)
{
    DrawRectangleRounded(
        frame,
        0.05f,
        8,
        (Color){20, 24, 40, 235}
    );

    Rectangle inner =
    {
        frame.x + 8,
        frame.y + 8,
        frame.width - 16,
        frame.height - 16
    };

    if (texture.id != 0)
    {
        float side =
            (float)(texture.width < texture.height
                ? texture.width
                : texture.height);

        Rectangle source =
        {
            (texture.width - side) / 2.0f,
            (texture.height - side) / 2.0f,
            side,
            side
        };

        DrawTexturePro(
            texture,
            source,
            inner,
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
    }
    else
    {
        DrawRectangleRec(inner, (Color){35, 40, 64, 255});

        DrawText(
            "PHOTO",
            (int)(inner.x + (inner.width - MeasureText("PHOTO", 24)) / 2),
            (int)(inner.y + inner.height / 2 - 12),
            24,
            GRAY
        );
    }

    DrawRectangleRoundedLines(
        frame,
        0.05f,
        8,
        accent
    );
}

static void DrawCenteredAt(
    const char *text,
    int centerX,
    int y,
    int fontSize,
    Color color
)
{
    DrawText(
        text,
        centerX - MeasureText(text, fontSize) / 2,
        y,
        fontSize,
        color
    );
}

static MenuResult ShowAbout(void)
{
    Texture2D photoLeft =
        LoadTexture(ResolveMenuAsset(ABOUT_PHOTO_LEFT));

    Texture2D photoRight =
        LoadTexture(ResolveMenuAsset(ABOUT_PHOTO_RIGHT));

    if (photoLeft.id != 0)
        SetTextureFilter(photoLeft, TEXTURE_FILTER_BILINEAR);

    if (photoRight.id != 0)
        SetTextureFilter(photoRight, TEXTURE_FILTER_BILINEAR);

    Rectangle backButton = {350, 630, 130, 45};

    MenuResult result = MENU_ABOUT;

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        bool backHover =
            CheckCollisionPointRec(mouse, backButton);

        int centerX = GetScreenWidth() / 2;

        Rectangle leftFrame =
            {(float)(centerX - 190 - 120), 265, 240, 240};

        Rectangle rightFrame =
            {(float)(centerX + 190 - 120), 265, 240, 240};

        BeginDrawing();

        ClearBackground((Color){10, 10, 18, 255});

        DrawHeader();

        DrawCenteredAt("ABOUT US", centerX, 212, 25, GOLD);

        DrawAboutPhoto(photoLeft, leftFrame, GOLD);
        DrawAboutPhoto(photoRight, rightFrame, SKYBLUE);

        DrawCenteredAt(
            "ANKON MANDAL",
            (int)(leftFrame.x + leftFrame.width / 2),
            525,
            22,
            WHITE
        );

        DrawCenteredAt(
            "BUET CSE",
            (int)(leftFrame.x + leftFrame.width / 2),
            553,
            18,
            GOLD
        );

        DrawCenteredAt(
            "SAYON BASAK",
            (int)(rightFrame.x + rightFrame.width / 2),
            525,
            22,
            WHITE
        );

        DrawCenteredAt(
            "BUET CSE",
            (int)(rightFrame.x + rightFrame.width / 2),
            553,
            18,
            SKYBLUE
        );

        DrawCenteredAt(
            "THE MINDS BEHIND GRIDLOCK7",
            centerX,
            592,
            16,
            GRAY
        );

        DrawButton(
            backButton,
            "BACK",
            backHover,
            SKYBLUE
        );

        EndDrawing();

        if (
            backHover &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        )
        {
            break;
        }

        if (
            IsKeyPressed(KEY_ENTER) ||
            IsKeyPressed(KEY_ESCAPE)
        )
        {
            break;
        }
    }

    if (WindowShouldClose())
        result = MENU_EXIT;

    if (photoLeft.id != 0)
        UnloadTexture(photoLeft);

    if (photoRight.id != 0)
        UnloadTexture(photoRight);

    return result;
}

void ShowIntro(void)
{
    introTexture =
        LoadTexture(
            ResolveMenuAsset("sokoban_intro.png")
        );

    float introTime = 4.0f;

    while (
        !WindowShouldClose() &&
        introTime > 0.0f
    )
    {
        float dt = GetFrameTime();

        introTime -= dt;

        BeginDrawing();

        ClearBackground(BLACK);

        DrawFullScreenTexture(introTexture);

        EndDrawing();

        if (
            IsKeyPressed(KEY_ENTER) ||
            IsKeyPressed(KEY_SPACE) ||
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        )
        {
            break;
        }
    }

    if (introTexture.id != 0)
    {
        UnloadTexture(introTexture);
        introTexture = (Texture2D){0};
    }
}

MenuResult ShowMainMenu(
    const char *levelNames[LEVEL_COUNT],
    BestRecord records[LEVEL_COUNT]
)
{
    menuBackground =
        LoadTexture(
            ResolveMenuAsset("menu_bg.png")
        );

    Rectangle playButton =
        {280, 225, 270, 50};

    Rectangle helpButton =
        {280, 295, 270, 50};

    Rectangle leaderboardButton =
        {280, 365, 270, 50};

    Rectangle aboutButton =
        {280, 435, 270, 50};

    Rectangle exitButton =
        {280, 505, 270, 50};

    bool ignoreInput = false;

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        bool playHover =
            CheckCollisionPointRec(
                mouse,
                playButton
            );

        bool helpHover =
            CheckCollisionPointRec(
                mouse,
                helpButton
            );

        bool leaderboardHover =
            CheckCollisionPointRec(
                mouse,
                leaderboardButton
            );

        bool aboutHover =
            CheckCollisionPointRec(
                mouse,
                aboutButton
            );

        bool exitHover =
            CheckCollisionPointRec(
                mouse,
                exitButton
            );

        if (
            !ignoreInput &&
            playHover &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        )
        {
            if (menuBackground.id != 0)
                UnloadTexture(menuBackground);

            menuBackground = (Texture2D){0};

            return MENU_PLAY;
        }

        if (
            !ignoreInput &&
            helpHover &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        )
        {
            MenuResult result = ShowHelp();
            ignoreInput = true;

            if (result == MENU_EXIT)
            {
                if (menuBackground.id != 0)
                    UnloadTexture(menuBackground);

                menuBackground = (Texture2D){0};

                return MENU_EXIT;
            }
        }

        if (
            !ignoreInput &&
            leaderboardHover &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        )
        {
            ShowLeaderboard(
                levelNames,
                records
            );
            ignoreInput = true;
        }

        if (
            !ignoreInput &&
            aboutHover &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        )
        {
            MenuResult result = ShowAbout();
            ignoreInput = true;

            if (result == MENU_EXIT)
            {
                if (menuBackground.id != 0)
                    UnloadTexture(menuBackground);

                menuBackground = (Texture2D){0};

                return MENU_EXIT;
            }
        }

        if (
            !ignoreInput &&
            exitHover &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        )
        {
            if (menuBackground.id != 0)
                UnloadTexture(menuBackground);

            menuBackground = (Texture2D){0};

            return MENU_EXIT;
        }

        if (!ignoreInput && IsKeyPressed(KEY_ENTER))
        {
            if (menuBackground.id != 0)
                UnloadTexture(menuBackground);

            menuBackground = (Texture2D){0};

            return MENU_PLAY;
        }

        if (!ignoreInput && IsKeyPressed(KEY_ESCAPE))
        {
            if (menuBackground.id != 0)
                UnloadTexture(menuBackground);

            menuBackground = (Texture2D){0};

            return MENU_EXIT;
        }

        BeginDrawing();

        ClearBackground((Color){10, 10, 18, 255});

        if (menuBackground.id != 0)
        {
            DrawFullScreenTexture(menuBackground);
        }

        DrawRectangle(
            0,
            0,
            GetScreenWidth(),
            GetScreenHeight(),
            (Color){0, 0, 0, 70}
        );

        DrawHeader();

        DrawButton(
            playButton,
            "PLAY",
            playHover,
            GOLD
        );

        DrawButton(
            helpButton,
            "HELP",
            helpHover,
            SKYBLUE
        );

        DrawButton(
            leaderboardButton,
            "LEADERBOARD",
            leaderboardHover,
            LIME
        );

        DrawButton(
            aboutButton,
            "ABOUT US",
            aboutHover,
            GOLD
        );

        DrawButton(
            exitButton,
            "EXIT",
            exitHover,
            RED
        );

        DrawText(
            "Mouse: Select     ENTER: Play     ESC: Exit",
            255,
            580,
            15,
            GRAY
        );

        EndDrawing();

        ignoreInput = false;
    }

    if (menuBackground.id != 0)
        UnloadTexture(menuBackground);

    menuBackground = (Texture2D){0};

    return MENU_EXIT;
}

MenuResult ShowLevelSelect(
    const char *levelNames[LEVEL_COUNT],
    BestRecord records[LEVEL_COUNT]
)
{
    Rectangle buttons[LEVEL_COUNT];

    const float buttonW = 350;
    const float gap = 40;
    const float gridW = buttonW * 2 + gap;
    const float startX = (GetScreenWidth() - gridW) / 2.0f;

    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        int column = i % 2;
        int row = i / 2;

        float x = startX + column * (buttonW + gap);

        if (i == LEVEL_COUNT - 1 && LEVEL_COUNT % 2 == 1)
            x = (GetScreenWidth() - buttonW) / 2.0f;

        buttons[i] =
            (Rectangle){
                x,
                250 + row * 98,
                buttonW,
                82
            };
    }

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        BeginDrawing();

        ClearBackground((Color){10, 10, 18, 255});

        DrawHeader();

        DrawText(
            "SELECT YOUR BATTLEFIELD",
            (GetScreenWidth() - MeasureText("SELECT YOUR BATTLEFIELD", 23)) / 2,
            210,
            23,
            SKYBLUE
        );

        for (int i = 0; i < LEVEL_COUNT; i++)
        {
            bool hovered =
                CheckCollisionPointRec(
                    mouse,
                    buttons[i]
                );

            DrawRectangleRounded(
                buttons[i],
                0.16f,
                8,
                hovered ? (Color){65, 75, 112, 240} : (Color){35, 40, 64, 230}
            );

            DrawRectangleLinesEx(buttons[i], 2, hovered ? (i == 0 ? GOLD : SKYBLUE) : GRAY);

            DrawText(levelNames[i], (int)buttons[i].x + 14, (int)buttons[i].y + 10, 18, WHITE);

            const char *recordLabel = records[i].hasRecord
                ? TextFormat("BEST: %s  %.2fs", records[i].name, records[i].time)
                : "BEST: --";

            DrawText(
                recordLabel,
                (int)buttons[i].x + 14,
                (int)buttons[i].y + 48,
                12,
                LIME
            );

        }

        DrawText(
            "ESC: Back",
            (GetScreenWidth() - MeasureText("ESC: Back", 16)) / 2,
            650,
            16,
            GRAY
        );

        EndDrawing();

        for (int i = 0; i < LEVEL_COUNT; i++)
        {
            if (
                CheckCollisionPointRec(
                    mouse,
                    buttons[i]
                ) &&
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
            )
            {
                return (MenuResult)(
                    MENU_LEVEL_1 + i
                );
            }
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            return MENU_EXIT;
        }
    }

    return MENU_EXIT;
}

bool ShowNameEntry(
    char *name,
    int maxLength,
    const char *levelName
)
{
    int length = 0;

    name[0] = '\0';

    Rectangle inputBox =
        {190, 325, 580, 60};

    Rectangle startButton =
        {325, 420, 250, 55};

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        bool startHover =
            CheckCollisionPointRec(
                mouse,
                startButton
            );

        int key;

        while ((key = GetCharPressed()) != 0)
        {
            if (
                key >= 32 &&
                key <= 126 &&
                key != 8 &&
                length < maxLength - 1
            )
            {
                name[length++] = (char)key;
                name[length] = '\0';
            }
        }

        if (
            IsKeyPressed(KEY_BACKSPACE) &&
            length > 0
        )
        {
            length--;
            name[length] = '\0';
        }

        if (
            (
                IsKeyPressed(KEY_ENTER) ||
                (
                    startHover &&
                    IsMouseButtonPressed(
                        MOUSE_BUTTON_LEFT
                    )
                )
            ) &&
            length > 0
        )
        {
            return true;
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            return false;
        }

        BeginDrawing();

        ClearBackground((Color){10, 10, 18, 255});

        DrawHeader();

        DrawText(
            "IDENTIFY YOURSELF",
            (GetScreenWidth() - MeasureText("IDENTIFY YOURSELF", 25)) / 2,
            215,
            25,
            GOLD
        );

        DrawText(
            levelName,
            (GetScreenWidth() - MeasureText(levelName, 18)) / 2,
            253,
            18,
            SKYBLUE
        );

        DrawText(
            "ENTER YOUR NAME BEFORE THE TIMER STARTS",
            (GetScreenWidth() - MeasureText("ENTER YOUR NAME BEFORE THE TIMER STARTS", 16)) / 2,
            285,
            16,
            GRAY
        );

        DrawRectangleRounded(
            inputBox,
            0.15f,
            8,
            (Color){25, 30, 50, 255}
        );

        DrawRectangleLinesEx(
            inputBox,
            2,
            SKYBLUE
        );

        DrawText(
            name,
            210,
            344,
            22,
            WHITE
        );

        if (length == 0)
        {
            DrawText(
                "Type your name...",
                210,
                344,
                22,
                GRAY
            );
        }

        DrawButton(
            startButton,
            "START TIMER",
            startHover,
            LIME
        );

        DrawText(
            "ENTER: Start     BACKSPACE: Delete     ESC: Back",
            (GetScreenWidth() - MeasureText("ENTER: Start     BACKSPACE: Delete     ESC: Back", 15)) / 2,
            505,
            15,
            GRAY
        );

        EndDrawing();
    }

    return false;
}

void ShowLeaderboard(
    const char *levelNames[LEVEL_COUNT],
    BestRecord records[LEVEL_COUNT]
)
{
    Rectangle backButton =
        {350, 635, 130, 45};

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        bool backHover =
            CheckCollisionPointRec(
                mouse,
                backButton
            );

        BeginDrawing();

        ClearBackground((Color){10, 10, 18, 255});

        DrawHeader();

        DrawText(
            "FASTEST SOLVERS",
            (GetScreenWidth() - MeasureText("FASTEST SOLVERS", 23)) / 2,
            210,
            23,
            LIME
        );

        for (int i = 0; i < LEVEL_COUNT; i++)
        {
            int y = 268 + i * 47;

            DrawRectangleRounded(
                (Rectangle){85, (float)(y - 7), 662, 39},
                0.18f,
                6,
                (i % 2 == 0) ? (Color){25, 30, 50, 230} : (Color){17, 21, 37, 230}
            );

            DrawText(
                levelNames[i],
                100,
                y,
                15,
                WHITE
            );

            DrawSmallRecord(
                records[i],
                500,
                y
            );
        }

        DrawButton(
            backButton,
            "BACK",
            backHover,
            SKYBLUE
        );

        EndDrawing();

        if (
            backHover &&
            IsMouseButtonPressed(
                MOUSE_BUTTON_LEFT
            )
        )
        {
            return;
        }

        if (
            IsKeyPressed(KEY_ENTER) ||
            IsKeyPressed(KEY_ESCAPE)
        )
        {
            return;
        }
    }
}
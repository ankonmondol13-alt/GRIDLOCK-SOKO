#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "menu.h"

#if (RAYLIB_VERSION_MAJOR < 5) || (RAYLIB_VERSION_MAJOR == 5 && RAYLIB_VERSION_MINOR < 5)
#define IsMusicValid IsMusicReady
#endif

#define MAP_W 16
#define MAP_H 12
#define LEVEL_COUNT 7
#define TILE 52
#define PANEL 90
#define SCREEN_W (MAP_W * TILE)
#define SCREEN_H (MAP_H * TILE + PANEL)
#define RECORD_FILE "gridlock7_records.txt"

typedef struct
{
    int row;
    int col;
} Point;

const char *levelNames[LEVEL_COUNT] =
{
    "01 // EASY? LOL.",
    "02 // THREE'S COMPANY.",
    "03 // TIGHT CORNERS.",
    "04 // BOXED IN.",
    "05 // GOOD LUCK, BESTIE.",
    "06 // CHAOS ROOM.",
    "07 // THE OLD TRAP."
};

const char *levelHints[LEVEL_COUNT] =
{
    "Don't rush it. One stupid push can turn this tiny room into a dead end.",
    "Three crates. Several goals. Plan your pushes carefully.",
    "Tight corners make every move important. Think before you push.",
    "The walls leave very little space. One bad push can ruin everything.",
    "Four crates. Almost no breathing room. Every push needs a plan.",
    "Multiple crates and narrow paths. Keep the whole board in mind.",
    "Looks familiar? Don't let that fool you. The final trap is waiting."
};

const char *levels[LEVEL_COUNT][MAP_H] =
{
    {
        "    ###         ",
        "   #   #   #    ",
        "   # $ #  # #   ",
        "   #@$ #  #.#   ",
        "   # $ ####.#   ",
        "    #      .#   ",
        "    #  #### #   ",
        "     # #  # #   ",
        "     # #  # #   ",
        "     # #### #   ",
        "     #      #   ",
        "      ######    "
    },
    {
        "                ",
        "                ",
        "                ",
        "      ####      ",
        "     #    #     ",
        "    ##$$$ #     ",
        "   #@ $.. #     ",
        "   # $...#      ",
        "    ###  #      ",
        "       ##       ",
        "                ",
        "                "
    },
    {
        "                ",
        "                ",
        "       ###      ",
        "     ##  @#     ",
        "    #  $. #     ",
        "    #  .$. #    ",
        "     # $.$ #    ",
        "      #   #     ",
        "       ###      ",
        "                ",
        "                ",
        "                "
    },
    {
        "                ",
        "                ",
        "     ####       ",
        "    #    #      ",
        "    # $$$#      ",
        "    #  #..##    ",
        "     #  ..$ #   ",
        "     #@     #   ",
        "      ######    ",
        "                ",
        "                ",
        "                "
    },
    {
        "                ",
        "                ",
        "      ###       ",
        "     # @ ##     ",
        "     #$#$  #    ",
        "    # .. . #    ",
        "    #  $$ #     ",
        "     ## #.#     ",
        "      #   #     ",
        "       ###      ",
        "                ",
        "                "
    },
    {
        "                ",
        "                ",
        "                ",
        "     ###        ",
        "    #   ####    ",
        "    # # #   #   ",
        "    # $   $ #   ",
        "    #..#$#$#    ",
        "    #.@$   #    ",
        "    #..  ##     ",
        "     ####       ",
        "                "
    },
    {
        "                ",
        "                ",
        "      ####      ",
        "     #    #     ",
        "     #.##$ #    ",
        "    # ..$  #    ",
        "    #  #$  #    ",
        "    #  @ ##     ",
        "     ####       ",
        "                ",
        "                ",
        "                "
    }
};

char board[MAP_H][MAP_W];
bool goal[MAP_H][MAP_W];

Point player;

int levelIndex = 0;
int moves = 0;
int pushes = 0;

float bannerTime = 0.0f;

bool completed = false;
bool boxed = false;
bool newRecord = false;

double gameStartTime = 0.0;
double finalTime = 0.0;
double currentTime = 0.0;

char playerName[MAX_NAME_LENGTH] = "";

BestRecord records[LEVEL_COUNT];

Texture2D mapTexture = {0};

const char *ResolveAsset(const char *name)
{
    static char resolved[1024];

    if (FileExists(name))
    {
        return name;
    }

    TextCopy(
        resolved,
        TextFormat("%s%s", GetApplicationDirectory(), name)
    );

    if (FileExists(resolved))
    {
        return resolved;
    }

    return name;
}

bool Inside(int r, int c)
{
    return r >= 0 &&
           r < MAP_H &&
           c >= 0 &&
           c < MAP_W;
}

bool CrateAt(int r, int c)
{
    return Inside(r, c) &&
           board[r][c] == '$';
}

bool WallAt(int r, int c)
{
    return !Inside(r, c) ||
           board[r][c] == '#';
}

void LoadRecords(void)
{
    FILE *file = fopen(
        ResolveAsset(RECORD_FILE),
        "r"
    );

    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        records[i].name[0] = '\0';
        records[i].time = 0.0;
        records[i].hasRecord = false;
    }

    if (!file)
    {
        return;
    }

    int level;
    double time;
    char name[MAX_NAME_LENGTH];

    while (
        fscanf(
            file,
            "%d %lf %23[^\n]",
            &level,
            &time,
            name
        ) == 3
    )
    {
        if (
            level >= 0 &&
            level < LEVEL_COUNT
        )
        {
            TextCopy(
                records[level].name,
                name
            );

            records[level].time = time;
            records[level].hasRecord = true;
        }
    }

    fclose(file);
}

void SaveRecords(void)
{
    char path[1024];

    if (FileExists(RECORD_FILE))
    {
        TextCopy(
            path,
            RECORD_FILE
        );
    }
    else
    {
        TextCopy(
            path,
            TextFormat(
                "%s%s",
                GetApplicationDirectory(),
                RECORD_FILE
            )
        );
    }

    FILE *file = fopen(path, "w");

    if (!file)
    {
        return;
    }

    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        if (records[i].hasRecord)
        {
            fprintf(
                file,
                "%d %.6f %s\n",
                i,
                records[i].time,
                records[i].name
            );
        }
    }

    fclose(file);
}

void UpdateBestRecord(void)
{
    if (
        !records[levelIndex].hasRecord ||
        finalTime < records[levelIndex].time
    )
    {
        TextCopy(
            records[levelIndex].name,
            playerName
        );

        records[levelIndex].time = finalTime;
        records[levelIndex].hasRecord = true;

        newRecord = true;

        SaveRecords();
    }
    else
    {
        newRecord = false;
    }
}

void StartTimer(void)
{
    gameStartTime = GetTime();
    currentTime = 0.0;
    finalTime = 0.0;
}

void LoadLevel(int index)
{
    levelIndex = index;
    moves = 0;
    pushes = 0;
    completed = false;
    boxed = false;
    newRecord = false;

    bannerTime = 6.0f;

    StartTimer();

    for (int r = 0; r < MAP_H; r++)
    {
        for (int c = 0; c < MAP_W; c++)
        {
            char ch = levels[index][r][c];

            goal[r][c] = (ch == '.');
            board[r][c] =
                (ch == '.') ? ' ' : ch;

            if (ch == '@')
            {
                player = (Point){r, c};
            }
        }
    }
}

bool IsWin(void)
{
    int goals = 0;
    int parked = 0;

    for (int r = 0; r < MAP_H; r++)
    {
        for (int c = 0; c < MAP_W; c++)
        {
            if (goal[r][c])
            {
                goals++;

                if (CrateAt(r, c))
                {
                    parked++;
                }
            }
        }
    }

    return goals > 0 &&
           goals == parked;
}

bool IsDeadCorner(int r, int c)
{
    if (
        !Inside(r, c) ||
        goal[r][c] ||
        !CrateAt(r, c)
    )
    {
        return false;
    }

    bool up = WallAt(r - 1, c);
    bool down = WallAt(r + 1, c);
    bool left = WallAt(r, c - 1);
    bool right = WallAt(r, c + 1);

    return (up && left) ||
           (up && right) ||
           (down && left) ||
           (down && right);
}

void TryMove(int dr, int dc)
{
    if (completed || boxed)
    {
        return;
    }

    int nr = player.row + dr;
    int nc = player.col + dc;

    if (WallAt(nr, nc))
    {
        return;
    }

    if (CrateAt(nr, nc))
    {
        int br = nr + dr;
        int bc = nc + dc;

        if (
            WallAt(br, bc) ||
            CrateAt(br, bc)
        )
        {
            return;
        }
    }

    if (CrateAt(nr, nc))
    {
        int br = nr + dr;
        int bc = nc + dc;

        board[br][bc] = '$';
        board[nr][nc] = ' ';

        pushes++;

        if (IsDeadCorner(br, bc))
        {
            boxed = true;
        }
    }

    board[player.row][player.col] = ' ';

    player = (Point){nr, nc};

    board[player.row][player.col] = '@';

    moves++;

    if (IsWin())
    {
        finalTime =
            GetTime() - gameStartTime;

        currentTime = finalTime;

        completed = true;
        boxed = false;

        UpdateBestRecord();
    }
}

void DrawGoal(int r, int c)
{
    int x = c * TILE;
    int y = r * TILE;

    Vector2 center =
    {
        x + TILE / 2.0f,
        y + TILE / 2.0f
    };

    DrawCircle(
        (int)center.x,
        (int)center.y + 3,
        20,
        (Color){8, 35, 27, 220}
    );

    DrawCircleGradient(
        center,
        17.0f,
        (Color){100, 255, 160, 255},
        DARKGREEN
    );

    DrawCircleLines(
        (int)center.x,
        (int)center.y,
        17,
        (Color){220, 255, 220, 230}
    );

    DrawCircle(
        (int)center.x,
        (int)center.y,
        7,
        (Color){8, 70, 45, 255}
    );

    DrawCircle(
        (int)center.x,
        (int)center.y,
        3,
        WHITE
    );
}

void DrawMapTexture(void)
{
    if (mapTexture.id != 0)
    {
        Rectangle source =
        {
            0,
            0,
            (float)mapTexture.width,
            (float)mapTexture.height
        };

        Rectangle destination =
        {
            0,
            0,
            SCREEN_W,
            MAP_H * TILE
        };

        DrawTexturePro(
            mapTexture,
            source,
            destination,
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
    }
    else
    {
        DrawRectangle(
            0,
            0,
            SCREEN_W,
            MAP_H * TILE,
            (Color){20, 24, 35, 255}
        );
    }

    for (int r = 0; r < MAP_H; r++)
    {
        for (int c = 0; c < MAP_W; c++)
        {
            if (board[r][c] == '#')
            {
                int x = c * TILE;
                int y = r * TILE;

                DrawRectangle(
                    x,
                    y,
                    TILE - 1,
                    TILE - 1,
                    (Color){40, 50, 75, 235}
                );

                DrawRectangleLines(
                    x,
                    y,
                    TILE - 1,
                    TILE - 1,
                    (Color){90, 105, 135, 220}
                );
            }
        }
    }
}

void DrawTimer(void)
{
    int minutes =
        (int)(currentTime / 60.0);

    double seconds =
        currentTime -
        minutes * 60.0;

    DrawText(
        TextFormat(
            "TIME %02i:%05.2f",
            minutes,
            seconds
        ),
        520,
        MAP_H * TILE + 40,
        18,
        LIME
    );
}

void DrawGame(void)
{
    ClearBackground(
        (Color){10, 10, 15, 255}
    );

    DrawMapTexture();

    for (int r = 0; r < MAP_H; r++)
    {
        for (int c = 0; c < MAP_W; c++)
        {
            if (
                goal[r][c] &&
                !CrateAt(r, c)
            )
            {
                DrawGoal(r, c);
            }
        }
    }

    for (int r = 0; r < MAP_H; r++)
    {
        for (int c = 0; c < MAP_W; c++)
        {
            if (board[r][c] == '$')
            {
                int x = c * TILE;
                int y = r * TILE;

                Color crateColor =
                    goal[r][c]
                    ? LIME
                    : GOLD;

                DrawRectangle(
                    x + 6,
                    y + 6,
                    TILE - 12,
                    TILE - 12,
                    crateColor
                );

                DrawRectangleLines(
                    x + 6,
                    y + 6,
                    TILE - 12,
                    TILE - 12,
                    BROWN
                );

                Rectangle crate =
                {
                    x + 6,
                    y + 7,
                    TILE - 12,
                    TILE - 12
                };

                Color edge =
                    goal[r][c]
                    ? DARKGREEN
                    : (Color){93, 49, 16, 255};

                DrawRectangleRounded(
                    (Rectangle){
                        x + 8,
                        y + 10,
                        TILE - 12,
                        TILE - 12
                    },
                    0.16f,
                    6,
                    (Color){0, 0, 0, 90}
                );

                DrawRectangleGradientEx(
                    crate,
                    crateColor,
                    crateColor,
                    (Color){150, 76, 18, 255},
                    (Color){210, 119, 28, 255}
                );

                DrawRectangleLinesEx(
                    crate,
                    2,
                    edge
                );

                DrawRectangleLinesEx(
                    (Rectangle){
                        x + 11,
                        y + 12,
                        TILE - 22,
                        TILE - 22
                    },
                    1,
                    (Color){255, 230, 130, 170}
                );

                DrawLine(
                    x + 12,
                    y + 13,
                    x + TILE - 12,
                    y + TILE - 13,
                    edge
                );

                DrawLine(
                    x + TILE - 12,
                    y + 13,
                    x + 12,
                    y + TILE - 13,
                    edge
                );
            }
        }
    }

    int px =
        player.col * TILE +
        TILE / 2;

    int py =
        player.row * TILE +
        TILE / 2;

    DrawCircle(
        px,
        py,
        18,
        SKYBLUE
    );

    DrawCircle(
        px,
        py,
        6,
        WHITE
    );

    DrawCircle(
        px + 2,
        py + 4,
        19,
        (Color){0, 0, 0, 100}
    );

    DrawCircle(
        px,
        py,
        18,
        (Color){42, 120, 220, 255}
    );

    DrawCircle(
        px,
        py - 4,
        11,
        (Color){255, 211, 166, 255}
    );

    DrawCircle(
        px - 4,
        py - 5,
        2,
        DARKBROWN
    );

    DrawCircle(
        px + 4,
        py - 5,
        2,
        DARKBROWN
    );

    DrawLine(
        px - 5,
        py + 2,
        px + 5,
        py + 2,
        MAROON
    );

    DrawRectangleRounded(
        (Rectangle){
            px - 12,
            py + 7,
            24,
            12
        },
        0.4f,
        5,
        (Color){35, 70, 175, 255}
    );

    DrawCircleLines(
        px,
        py,
        18,
        (Color){190, 230, 255, 230}
    );

    DrawRectangle(
        0,
        MAP_H * TILE,
        SCREEN_W,
        PANEL,
        (Color){15, 15, 22, 255}
    );

    DrawText(
        "GRIDLOCK7 SOKOBAN",
        20,
        MAP_H * TILE + 10,
        20,
        RED
    );

    DrawText(
        levelNames[levelIndex],
        20,
        MAP_H * TILE + 37,
        16,
        GOLD
    );

    DrawText(
        TextFormat(
            "PLAYER: %s",
            playerName
        ),
        20,
        MAP_H * TILE + 62,
        14,
        SKYBLUE
    );

    DrawText(
        TextFormat(
            "MOVES: %03i PUSHES: %03i",
            moves,
            pushes
        ),
        520,
        MAP_H * TILE + 15,
        16,
        WHITE
    );

    DrawTimer();

    if (bannerTime > 0 && !completed)
    {
        DrawRectangle(
            100,
            220,
            632,
            60,
            (Color){0, 0, 0, 220}
        );

        DrawRectangleLines(
            100,
            220,
            632,
            60,
            RED
        );

        DrawText(
            levelNames[levelIndex],
            110,
            230,
            18,
            GOLD
        );

        DrawText(
            levelHints[levelIndex],
            110,
            252,
            14,
            WHITE
        );
    }

    if (boxed)
    {
        DrawRectangle(
            110,
            190,
            612,
            150,
            (Color){55, 0, 0, 235}
        );

        DrawRectangleLines(
            110,
            190,
            612,
            150,
            RED
        );

        DrawText(
            "YOU ARE BOXED ALREADY!",
            245,
            210,
            24,
            RED
        );

        DrawText(
            "That crate is stuck in a corner.",
            245,
            250,
            16,
            WHITE
        );

        DrawText(
            "The puzzle cannot be completed from here.",
            190,
            275,
            16,
            WHITE
        );

        DrawText(
            "Press R to restart the game.",
            255,
            305,
            17,
            YELLOW
        );
    }

    if (completed)
    {
        DrawRectangle(
            125,
            165,
            582,
            190,
            (Color){0, 40, 20, 240}
        );

        DrawRectangleLines(
            125,
            165,
            582,
            190,
            LIME
        );

        DrawText(
            "SECTOR CLEARED!",
            270,
            185,
            24,
            GREEN
        );

        DrawText(
            TextFormat(
                "SOLVER: %s",
                playerName
            ),
            285,
            220,
            16,
            WHITE
        );

        DrawText(
            TextFormat(
                "TIME: %.2f SECONDS",
                finalTime
            ),
            275,
            248,
            18,
            LIME
        );

        DrawText(
            TextFormat(
                "MOVES: %i | PUSHES: %i",
                moves,
                pushes
            ),
            270,
            278,
            15,
            WHITE
        );

        if (newRecord)
        {
            DrawText(
                "★ NEW LEVEL RECORD ★",
                275,
                307,
                18,
                GOLD
            );
        }
        else
        {
            DrawText(
                TextFormat(
                    "BEST: %s  %.2fs",
                    records[levelIndex].name,
                    records[levelIndex].time
                ),
                250,
                307,
                15,
                SKYBLUE
            );
        }

        DrawText(
            "PRESS ENTER FOR NEXT LEVEL",
            (SCREEN_W - MeasureText("PRESS ENTER FOR NEXT LEVEL", 14)) / 2,
            330,
            14,
            YELLOW
        );
    }
}

int main(void)
{
    InitWindow(
        SCREEN_W,
        SCREEN_H,
        "GRIDLOCK7 SOKOBAN"
    );

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    LoadRecords();
    ShowIntro();

    if (WindowShouldClose())
    {
        CloseWindow();
        return 0;
    }

    InitAudioDevice();

    Music bgMusic =
        LoadMusicStream(
            ResolveAsset("soko.mp3.mpeg.wav")
        );

    if (IsMusicValid(bgMusic))
    {
        PlayMusicStream(bgMusic);
    }

    mapTexture =
        LoadTexture(
            ResolveAsset("sokoban_map_texture.png")
        );

    bool running = true;

    while (running && !WindowShouldClose())
    {
        MenuResult result = ShowMainMenu(levelNames, records);

        if (result == MENU_EXIT || WindowShouldClose())
        {
            break;
        }

        if (result != MENU_PLAY)
        {
            continue;
        }

        result = ShowLevelSelect(levelNames, records);

        if (result == MENU_EXIT || WindowShouldClose())
        {
            continue;
        }

        int selectedLevel = result - MENU_LEVEL_1;

        if (selectedLevel < 0 || selectedLevel >= LEVEL_COUNT)
        {
            selectedLevel = 0;
        }

        if (!ShowNameEntry(playerName, MAX_NAME_LENGTH, levelNames[selectedLevel]))
        {
            continue;
        }

        LoadLevel(selectedLevel);

        bool quitToMenu = false;

        while (!WindowShouldClose() && !quitToMenu)
        {
            float dt = GetFrameTime();

            if (IsMusicValid(bgMusic))
            {
                UpdateMusicStream(bgMusic);
            }

            if (bannerTime > 0)
            {
                bannerTime -= dt;
            }

            if (!completed && !boxed)
            {
                currentTime = GetTime() - gameStartTime;
            }

            if (IsKeyPressed(KEY_ESCAPE))
            {
                quitToMenu = true;
            }

            if (IsKeyPressed(KEY_R))
            {
                LoadLevel(levelIndex);
            }

            if (
                IsKeyPressed(KEY_ENTER) &&
                completed &&
                levelIndex < LEVEL_COUNT - 1
            )
            {
                int nextLevel = levelIndex + 1;

                if (
                    ShowNameEntry(
                        playerName,
                        MAX_NAME_LENGTH,
                        levelNames[nextLevel]
                    )
                )
                {
                    LoadLevel(nextLevel);
                }
                else
                {
                    quitToMenu = true;
                }
            }

            if (!completed && !boxed)
            {
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
                {
                    TryMove(-1, 0);
                }
                else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
                {
                    TryMove(1, 0);
                }
                else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
                {
                    TryMove(0, -1);
                }
                else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                {
                    TryMove(0, 1);
                }
            }

            BeginDrawing();
            DrawGame();
            EndDrawing();
        }
    }

    UnloadTexture(mapTexture);

    if (IsMusicValid(bgMusic))
    {
        UnloadMusicStream(bgMusic);
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
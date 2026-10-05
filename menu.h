#ifndef MENU_H
#define MENU_H

#include "raylib.h"

#define MAX_NAME_LENGTH 24
#define LEVEL_COUNT 7

typedef enum
{
    MENU_PLAY,
    MENU_LEVEL_SELECT,
    MENU_LEVEL_1,
    MENU_LEVEL_2,
    MENU_LEVEL_3,
    MENU_LEVEL_4,
    MENU_LEVEL_5,
    MENU_LEVEL_6,
    MENU_LEVEL_7,
    MENU_HELP,
    MENU_LEADERBOARD,
    MENU_ABOUT,
    MENU_EXIT
} MenuResult;

typedef struct
{
    char name[MAX_NAME_LENGTH];
    double time;
    bool hasRecord;
} BestRecord;

void ShowIntro(void);

MenuResult ShowMainMenu(
    const char *levelNames[LEVEL_COUNT],
    BestRecord records[LEVEL_COUNT]
);

MenuResult ShowLevelSelect(
    const char *levelNames[LEVEL_COUNT],
    BestRecord records[LEVEL_COUNT]
);

bool ShowNameEntry(
    char *name,
    int maxLength,
    const char *levelName
);

void ShowLeaderboard(
    const char *levelNames[LEVEL_COUNT],
    BestRecord records[LEVEL_COUNT]
);

#endif
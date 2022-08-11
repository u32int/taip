#pragma once

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define VERSION "0.1"

#define SCR_WIDTH  1280
#define SCR_HEIGHT 720
#define TEXT_LINES 3

#define FPS_CAP 60
#define FPS_DELTA (1000/FPS_CAP)

extern int FONT_SIZE;

typedef enum {
  Default,
  DefaultLight,
  DarkYellow,
  SolarizedDark,

  ThemesCount /* must be the last elem */
} Theme;

typedef struct {
    const char* prettyName;
    SDL_Color bg;
    SDL_Color primary;
    SDL_Color error;
    SDL_Color dim;
} ThemeData;

typedef enum {
    Quit,
    Idle,
    InProgress,
    Results,
    Settings,
    Help,
} State;

typedef enum {
    Time,
    Words,
    Quote
} Mode;

typedef struct {
    bool showHints;

    int timeModeSeconds;
    int wordModeWords;

    const char *wordlistPath;
    const char *fontPath;
} GameSettings;

typedef struct {
    int errors;
    int totalCharCount;
} TestStats;

typedef struct {
    Uint64 timeStart;
    Uint64 timeEnd;
} Timers;

typedef enum {
    /* generic */
    BoolSwitch,
    IntSlider,
    IntCounter,
    /* special case */
    ThemeSelector,
} VisibleSettingType;

typedef struct {
    const char *label;
    void *settingPtr;
    VisibleSettingType type;
    union {
        /* int bounds */
        struct {
            int intMax, intMin;
            int intStep;
        };
        size_t strMaxLen;
    };
} VisibleSetting;

typedef struct {
    char txtBuff[TEXT_LINES][256];
    int lineLen;                  /* len and progress of the top/active line */
    int lineProgress;
    int errorIndex;               /* -1 if no error */

    GameSettings settings;
    VisibleSetting visibleSettings[32];
    int selSetting;
    size_t settingsCount;

    ThemeData *theme;
    int selTheme;

    State state;

    Mode mode;
    TestStats stats;
    Timers timers;
} game_t;

void init_game(game_t *game);
void reset_game(game_t *game);
void cycle_lines(game_t *game);

void timeModeStart(game_t *game);

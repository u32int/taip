#pragma once

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define VERSION "0.3"

#define SCR_WIDTH  1280
#define SCR_HEIGHT 720
#define TEXT_LINES 3

#define FPS_CAP 60
#define FPS_DELTA (1000/FPS_CAP)

extern int FONT_SIZE;
#define FONT_SIZE_MIN 16
#define FONT_SIZE_MAX 60

#define TIMEMODESECONDS_MIN 5
#define TIMEMODESECONDS_MAX 240

#define WORDSPERLINE_MIN 2
#define WORDSPERLINE_MAX 15

extern int win_w, win_h;

typedef struct {
    const char* prettyName;
    const char* filePath;
} Wordlist;

static const Wordlist wordlists[] = {
    { "English (monkeytype)",    "../wordlists/mt_english200.txt" },
    { "English 1k (monkeytype)", "../wordlists/mt_english1k.txt" },
    { "Polish 1k (monkeytype)",  "../wordlists/mt_polish200.txt" },
};

#define WordlistCount (sizeof(wordlists)/sizeof(Wordlist))

typedef struct {
    const char* prettyName;
    SDL_Color bg;
    SDL_Color dim;
    SDL_Color error;
    SDL_Color primary;
} Theme;

static const Theme themes[] = {
    { .prettyName = "Default",
      .bg      = {18, 18, 18, 255},
      .dim     = {60, 60, 60, 255},
      .error   = {240, 30, 20, 150},
      .primary = {240, 240, 240, 255} },

    { .prettyName = "Default Light",
      .bg      = {244, 244, 227, 255},
      .dim     = {160, 160, 160, 255},
      .error   = {240, 30, 20, 255},
      .primary = {22, 22, 22, 255} },

    { .prettyName = "Dark-Yellow",
      .bg      = {18, 18, 18, 255},
      .dim     = {60, 60, 60, 255},
      .error   = {240, 30, 20, 150},
      .primary = {240, 220, 35, 255} },

    { .prettyName = "Solarized Dark",
      .bg      = {0, 43, 54, 255},
      .dim     = {101, 123, 131, 255},
      .error   = {220, 50, 47, 150},
      .primary = {238, 232, 213, 255} },
};

#define ThemesCount (sizeof(themes)/sizeof(Theme))

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
    WordlistSelector,
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
    };
} VisibleSetting;

typedef struct {
    bool showHints;
    bool focusMode;
    int wordsPerLine;
    int timeModeSeconds;
    int wordModeWords;
    
    int selTheme;
    int selWordlist;

    const char *fontPath;
} GameSettings;

typedef struct {
    char txtBuff[TEXT_LINES][256];
    int lineLen;                  /* len and progress of the top/active line */
    int lineProgress;
    int errorIndex;               /* -1 if no error */

    GameSettings settings;
    VisibleSetting visibleSettings[32];
    int selSetting;
    int settingsCount;

    const Theme *theme;
    State state;
    Mode mode;
    TestStats stats;
    Timers timers;
} game_t;

void set_theme(game_t *game, int id);

void init_game(game_t *game);
void reset_game(game_t *game);
void cycle_lines(game_t *game);

void timeModeStart(game_t *game);

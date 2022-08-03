#pragma once

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define SCR_WIDTH  1280
#define SCR_HEIGHT 720
#define TEXT_LINES 3

#define FPS_CAP 60
#define FPS_DELTA (1000/FPS_CAP)

/* This is temporary */
#define FONT_SIZE 42

typedef enum {
  Default,
  DefaultLight,
} Theme;

typedef struct {
    SDL_Color bg;
    SDL_Color primary;
    SDL_Color error;
    SDL_Color dim;
} ThemeColors;

typedef enum {
    Quit,
    Idle,
    InProgress,
    Results,
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


typedef struct {
    int timeModeSeconds;
    int wordModeWords;
    char txtBuff[TEXT_LINES][256];
    int lineLen;                  /* len and progress of the top/active line */
    int lineProgress;
    const char *wordlistPath;
    const char *fontPath;
    int errorIndex;               /* -1 if no error */


    ThemeColors *theme;
    State state;
    Mode mode;
    TestStats stats;
    Timers timers;
} game_t;


void init_game(game_t *game);
void reset_game(game_t *game);
void cycle_lines(game_t *game);

void timeModeStart(game_t *game);

#include "game.h"
#include "logic.h"
#include "colors.h"
#include <stdio.h>
#include <string.h>

void init_game(game_t *game)
{
    /* default settings */
    game->mode = Time;
    
    game->settings.timeModeSeconds = 15;
    game->settings.wordModeWords = 50;
    game->settings.showHints = true;
    game->settings.wordlistPath = "../wordlists/mt_english200.txt";
    game->settings.fontPath = "../fonts/LiberationSans-Regular.ttf";

    set_theme(game, SolarizedDark);
    reset_game(game);
}

void reset_game(game_t *game)
{
    for (int i = 0; i < TEXT_LINES; ++i) {
        rand_line(game, i, game->settings.wordlistPath, 5);
    }
    game->state = Idle;
    game->lineLen = strlen(game->txtBuff[0]);

    game->lineProgress = 0;
    game->errorIndex = -1;

    game->stats.errors = 0;
    game->stats.totalCharCount = 0;
}

void cycle_lines(game_t *game)
{
    for (int i = 0; i < TEXT_LINES-1; ++i) {
        strcpy(game->txtBuff[i], game->txtBuff[i+1]);
    }
    game->stats.totalCharCount += game->lineLen;

    game->lineLen = strlen(game->txtBuff[0]);
    game->lineProgress = 0;

    game->errorIndex = -1;
    rand_line(game, TEXT_LINES-1, game->settings.wordlistPath, 5);
}

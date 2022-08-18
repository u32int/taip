#include "game.h"
#include "logic.h"
#include <stdio.h>
#include <string.h>

void set_theme(game_t *game, int id)
{
    game->settings.selTheme = id;
    game->theme = &themes[id];
}

void init_game(game_t *game)
{
    /* default settings */
    game->mode = Time;
    
    game->settings.timeModeSeconds = 30;
    game->settings.wordModeWords = 50;
    game->settings.showHints = true;
    game->settings.wordsPerLine = 5;
    game->settings.selWordlist = 0;
    game->settings.fontPath = "../fonts/LiberationSans-Regular.ttf";

    set_theme(game, 0);
    reset_game(game);
}

void reset_game(game_t *game)
{
    for (int i = 0; i < TEXT_LINES; ++i) {
        rand_line(game, i, wordlists[game->settings.selWordlist].filePath,
                  game->settings.wordsPerLine);
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
    rand_line(game, TEXT_LINES-1, wordlists[game->settings.selWordlist].filePath,
              game->settings.wordsPerLine);
}

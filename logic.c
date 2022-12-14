#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>

#include "game.h"
#include "logic.h"
#include "util.h"

static bool lock_input = false;

int next_char_distance(char *c)
{
    unsigned char mask[] = {128, 64, 32, 16, 8, 4, 2, 1};

    if ((c[0] & mask[0]) == 0) {
        return 1; /* since top bit is 0, text is ascii or code point is only one byte */
    }

    /* check how many bytes we need to move forward by to skip 1 utf-8 char.
       this works by checking the top bits of the first byte which indicate the
       byte length. */
    int i = 0;
    size_t dist = 0;
    while ((c[0] & mask[i++]) != 0 && i < 8) {
        dist++;
    }

    return dist;
}

int prev_char_distance(game_t *game, char *c)
{
    (void) game;

    if (isascii(c[-1])) {
        return 1;
    }

    // FIXME: this is a stopgap (handle utf8 backspace correctly)

    /* unsigned char mask[] = {128, 64, 32, 16, 8, 4, 2, 1}; */
    /* size_t dist = 1; */
    /* int i = -1; */
    /* while ((c[i] & mask[0]) != 0 && game->lineProgress + i >= 0) { */
    /*     dist++; */
    /* } */
    return 2;
}

void handle_text_input(game_t *game, char *input)
{
    if (lock_input) {
        lock_input = false;
        // puts("input blocked");
        return;
    }

    if(game->lineProgress == 0 && !(game->state == InProgress)) {
        game->state = InProgress;
        switch (game->mode) {
        case Time:
            timeModeStart(game);
            break;
        default: {}
        }
    }

    size_t input_len = strlen(input);
    if (strncmp(input, &game->txtBuff[0][game->lineProgress], input_len) != 0) {
        if (game->errorIndex == -1) {
            game->errorIndex = game->lineProgress;
        }
        game->stats.errors++;
    }
    game->lineProgress += next_char_distance(&game->txtBuff[0][game->lineProgress]);

    if (game->lineProgress >= game->lineLen) {
        cycle_lines(game);
    }
}

void handle_key(game_t *game, SDL_Keycode key, SDL_Keymod mod)
{
    switch (game->state) {
    case Idle:
        // fall through
    case InProgress:
        switch (mod) {
        case KMOD_NONE:
        case KMOD_LSHIFT:
        case KMOD_RSHIFT:
            switch (key) {
            case SDLK_BACKSPACE:
                if (game->lineProgress > 0) {
                    game->lineProgress -=
                        prev_char_distance(game, &game->txtBuff[0][game->lineProgress]);
                }

                if (game->lineProgress == game->errorIndex) {
                    game->errorIndex = -1;
                }
                break;
            default: {}
            }
            break;
        case KMOD_LCTRL:
            switch (key) {
            case SDLK_BACKSPACE: /* ctrl+backspace to delete a word */
                if (game->lineProgress != 0) {
                    game->lineProgress--;
                    while (game->txtBuff[0][game->lineProgress-1] != ' ' &&
                        game->lineProgress != 0) {
                        game->lineProgress--;
                    }

                    if (game->lineProgress <= game->errorIndex) {
                        game->errorIndex = -1;
                    }
                }
                break;
            default: {}
            }
            break;
        case KMOD_LALT:
            switch (key) {
            case SDLK_q:
                game->state = Quit;
                break;
            case SDLK_h:
                game->state = Help;
                break;
            case SDLK_s:
                game->state = Settings;
                break;
            case SDLK_r:
                reset_game(game);
                lock_input = true;
                break;
            default: {}
            }
        default: {}
        }
        break;

    case Results:
        if (mod == KMOD_LALT && key == SDLK_r) {
            reset_game(game);
            lock_input = true;
        }
        break;
    case Help:
        if (mod == KMOD_NONE && key == SDLK_ESCAPE)
            game->state = Idle;
        else if (mod == KMOD_LALT && key == SDLK_s)
            game->state = Settings;
        break;
    case Settings:
        switch(key) {
        case SDLK_q:
            lock_input = true;
            // fall through
        case SDLK_ESCAPE:
            game->state = Idle;
            reset_game(game);
            break;
        case SDLK_k:
        case SDLK_UP:
            game->selSetting = clamp_int(game->selSetting-1, 0, game->settingsCount);
            break;
        case SDLK_j:
        case SDLK_DOWN:
            game->selSetting = clamp_int(game->selSetting+1, 0, game->settingsCount);
            break;
        default: {}
        }

        VisibleSetting *curr_setting = &game->visibleSettings[game->selSetting];
        switch(curr_setting->type) {
        case BoolSwitch:
            switch (key) {
            case SDLK_LEFT:
            case SDLK_RIGHT:
            case SDLK_h:
            case SDLK_l:
            case SDLK_SPACE:
                *(bool*)curr_setting->settingPtr = !(*(bool*)curr_setting->settingPtr);
                break;
            default: {}
            }
            break;
        case ThemeSelector:
            switch (key) {
            case SDLK_h:
            case SDLK_LEFT:
                set_theme(game, clamp_int(*(int*)curr_setting->settingPtr-1,
                                          0, ThemesCount-1));
                break;
            case SDLK_l:
            case SDLK_RIGHT:
                set_theme(game, clamp_int(*(int*)curr_setting->settingPtr+1,
                                          0, ThemesCount-1));
                break;
            default: {}
            }
            break;
        case WordlistSelector:
            switch (key) {
            case SDLK_h:
            case SDLK_LEFT:
                *(int*)curr_setting->settingPtr =
                    clamp_int(*(int*)curr_setting->settingPtr-1, 0, WordlistCount-1);
                break;
            case SDLK_l:
            case SDLK_RIGHT:
                *(int*)curr_setting->settingPtr =
                    clamp_int(*(int*)curr_setting->settingPtr+1, 0, WordlistCount-1);
                break;
            default: {}
            }
            break;
        case IntCounter:
        case IntSlider:
            switch (key) {
            case SDLK_h:
            case SDLK_LEFT:
                *(int*)curr_setting->settingPtr =
                    clamp_int(*(int*)curr_setting->settingPtr-curr_setting->intStep,
                              curr_setting->intMin, curr_setting->intMax);
                break;
            case SDLK_l:
            case SDLK_RIGHT:
                *(int*)curr_setting->settingPtr =
                    clamp_int(*(int*)curr_setting->settingPtr+curr_setting->intStep,
                              curr_setting->intMin, curr_setting->intMax);
                break;
            default: {}
            }
            break;
        default: {}
        }
    default: {}
    }
}

size_t rand_word(FILE *file, int file_size, char *word_buff)
{

    /* fseek to somewhere in the file, find the nearest '\n',
       use that word as the random word. */
    int random_offset = rand() % file_size;
    fseek(file, random_offset, SEEK_SET);
    char c;
    while (!feof(file)) {
        c = fgetc(file);
        if (c == '\n')
            break;
    }
    size_t buffsize = 32;
    /* getline returns the amount of chars it read (-1 to exclude '\n') */
    return getline(&word_buff, &buffsize, file) - 1;
}

void rand_line(game_t *game, int index, const char* wordlist_name, const int words)
{
    FILE *file = fopen(wordlist_name, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening wordlist '%s': %s\n",
                wordlist_name, strerror(errno));
        exit(1);
    }
    /* get file size */
    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

    /* generate words */
    static bool capitalize_next;
    capitalize_next = index == 0 ? true : false;

    char text[256] = { 0 };
    char wbuff[32] = { 0 };
    char *curr = text;
    for(int i = 0; i < words; ++i) {
        size_t wlen = rand_word(file, file_size, wbuff);
        strncpy(curr, wbuff, 256-(curr-text));

        if (game->settings.punctuation) {
            if (capitalize_next) {
                *curr = toupper(*curr);
                capitalize_next = false;
            } else {
                int rand_punc = rand() % 10;
                if (rand_punc <= 2) {
                    text[curr-text+wlen] = '.';
                    capitalize_next = true;
                    wlen++;
                } else if (rand_punc <= 3) {
                    text[curr-text+wlen] = ',';
                    wlen++;
                }
            }
        }

        curr += wlen;
        *curr = ' ';
        curr++;
    }

    memcpy(game->txtBuff[index], text, 256);

    fclose(file);
}

void timeModeStart(game_t *game)
{
    game->timers.timeStart = SDL_GetTicks64();
    game->timers.timeEnd = SDL_GetTicks64() + game->settings.timeModeSeconds * 1000;
}

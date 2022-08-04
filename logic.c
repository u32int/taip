#define  _GNU_SOURCE

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "game.h"
#include "logic.h"

char get_key_ascii(SDL_Keycode key, bool uppercase)
{
    const char* s_key = SDL_GetKeyName(key);
    if(strlen(s_key) == 1) {
        return uppercase ? s_key[0] : 'a'+s_key[0]-'A'; /* convert to lowercase */
    };

    if(key == SDLK_SPACE)
        return ' ';
        
    return '\0';
}

void advance_line_progress(game_t *game, SDL_Keycode key, SDL_Keymod mod)
{
    if(game->lineProgress == 0 && !(game->state == InProgress)) {
        game->state = InProgress;
        switch (game->mode) {
        case Time:
            timeModeStart(game);
            break;
        default: {}
        }
    }

    char key_ascii = get_key_ascii(key, mod == KMOD_LSHIFT || mod == KMOD_RSHIFT);
    if (key_ascii == game->txtBuff[0][game->lineProgress]) {
        game->lineProgress++;
    } else if (key_ascii == '\0') {
        /* ignore */
    } else {
        if (game->errorIndex == -1) {
            game->errorIndex = game->lineProgress;
        }
        game->stats.errors++;
        game->lineProgress++;
    }

    if (game->lineProgress >= game->lineLen) {
        cycle_lines(game);
    }
}


void handle_key(game_t *game, SDL_Keycode key, SDL_Keymod mod)
{
    switch (game->state) {
    case Idle:
    case InProgress:
        switch (mod) {
        case KMOD_NONE:
        case KMOD_LSHIFT:
        case KMOD_RSHIFT:
            switch (key) {
            case SDLK_BACKSPACE: 
                if (game->lineProgress > 0) {
                    game->lineProgress--;
                }

                if (game->lineProgress == game->errorIndex) {
                    game->errorIndex = -1;
                }
                break;
            default:
                advance_line_progress(game, key, mod);
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
            case SDLK_r:
                reset_game(game);
                break;
            }
        default: {}
        }
        break;

    case Results:
        if (mod == KMOD_LALT && key == SDLK_r)
            reset_game(game);
    default: {}
    }
}

void rand_word(FILE *file, int file_size, char *word_buff)
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
    getline(&word_buff, &buffsize, file);
    word_buff[strcspn(word_buff, "\n")] = '\0'; /* remove trailing newline */
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
    char *text = (char*)calloc(256, sizeof(char)); 
    char *wbuff = (char*)calloc(32, sizeof(char));
    char *curr = text;
    for(int i = 0; i < words; ++i) {
        rand_word(file, file_size, wbuff);
        size_t wlen = strlen(wbuff);
        memcpy(curr, wbuff, wlen);
        curr += wlen;
        *curr = ' ';
        curr++;
    }

    memcpy(game->txtBuff[index], text, 256);

    free(text);
    free(wbuff);
}

void timeModeStart(game_t *game)
{
    game->timers.timeStart = SDL_GetTicks64();
    game->timers.timeEnd = SDL_GetTicks64() + game->timeModeSeconds * 1000;
}

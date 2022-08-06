#pragma once

#include <SDL2/SDL.h>
#include "game.h"

void handle_text_input(game_t *game, char *input);
void handle_key(game_t *game, SDL_Keycode key, SDL_Keymod mod);
void rand_line(game_t *game, int index, const char* wordlist_name, int words);

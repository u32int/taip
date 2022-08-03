#pragma once

#include "game.h"
#include <SDL2/SDL_ttf.h>

void render_game(SDL_Renderer *renderer, game_t *game, SDL_Window *window,
                 TTF_Font *font, TTF_Font *font_small);

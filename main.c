/*
  Copyright © 2022 u32int (aka vsh)

  This application is free software, distributed under the MIT license.

  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdio.h>
#include "time.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h> 

#include "game.h"
#include "render.h"
#include "logic.h"

#define WINDOW_NAME "taip"

void parse_args(int argc, char **argv, game_t *game)
{
    /* TODO add useful options and parse them (ex. setting timer seconds etc.) */
}

int main(int argc, char **argv)
{
    
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "sdl2 initialzation error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow(WINDOW_NAME,
                                          100, 100,
                                          SCR_WIDTH, SCR_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Couldn't create SDL_Window: %s\n", SDL_GetError()); 
        return EXIT_FAILURE;
    }

    SDL_SetWindowResizable(window, true);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED
                                                | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        fprintf(stderr, "Couldn't create SDL_Renderer: %s\n", SDL_GetError()); 
        return EXIT_FAILURE;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "sdl_ttf initialzation error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    game_t game;
    init_game(&game);
    parse_args(argc, argv, &game); /* args override defaults */

    TTF_Font *font = TTF_OpenFont(game.fontPath, FONT_SIZE);
    /* TTF_SetFontsize refuses to cooperate with me, hence font_small */
    /* this is TEMPORARY */
    TTF_Font *font_small = TTF_OpenFont(game.fontPath, FONT_SIZE/2);  
    if (font == NULL || font_small == NULL) {
        fprintf(stderr, "Font error: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Event e;
    Uint64 frame_start; 
    while (game.state != Quit) {
        frame_start = SDL_GetTicks64();
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                game.state = Quit;
                break;
            case SDL_KEYDOWN:
                handle_key(&game, e.key.keysym.sym, e.key.keysym.mod);
                break;
            default: {}
            }
        }

        /* if time is up in timeMode, show the results screen */
        if (game.state == InProgress && game.mode == Time) {
            if (SDL_GetTicks64() > game.timers.timeEnd) {
                game.stats.totalCharCount += game.lineProgress;
                game.state = Results;
            }
        }

        SDL_SetRenderDrawColor(renderer, game.theme->bg.r, game.theme->bg.g,
                               game.theme->bg.b, game.theme->bg.a);
        SDL_RenderClear(renderer);
        render_game(renderer, &game, window, font, font_small);
        SDL_RenderPresent(renderer);

        Uint64 frame_time = SDL_GetTicks64() - frame_start;
        if (frame_time < FPS_DELTA) {
            SDL_Delay(FPS_DELTA-frame_time);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_CloseFont(font);
    
    return EXIT_SUCCESS;
}

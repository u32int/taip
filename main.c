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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <getopt.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"
#include "render.h"
#include "logic.h"
#include "config.h"
#include "util.h"

#define WINDOW_NAME "taip"

int FONT_SIZE = 42;
int win_w, win_h;

void print_info_stdout()
{
  fprintf(stdout,
"taip v%s\n\
This application is free software, distributed under the MIT license.\n\
",
          VERSION);
}

void print_help_stdout()
{
  fputs("usage: taip [OPTION]\n\n\
Options:\n\
  -h, --help      print this message\n\
  -v, --version   print version and related information\n\
",
        stdout);
}

void parse_args(int argc, char **argv)
{
    /* using getopt, for more info see
       https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html */
    /* TODO add application settings related options */
    int c;
    while (1) {
        static struct option long_options[] = {
            { "help",    no_argument, 0, 'h'},
            { "version", no_argument, 0, 'v'},
            {0, 0, 0, 0}
        };

        int opt_index = 0;
        c = getopt_long(argc, argv, "hv", long_options, &opt_index);

        if (c == -1)
            break;

        switch (c) {
        case 'h':
            print_help_stdout();
            exit(0);
            break;
        case 'v':
            print_info_stdout();
            exit(0);
            break;
        default:
            exit(1);
        }
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    game_t game = {
        .visibleSettings = {
            {
              .label = "Show hints",
              .settingPtr = &game.settings.showHints,
              .type = BoolSwitch,
            },
            {
                /* When true, hides most additional information like currently selected
                   mode and wordlist in Idle mode. */
              .label = "Focus mode",
              .settingPtr = &game.settings.focusMode,
              .type = BoolSwitch,
            },
            {
                /* A counter is probably not the best idea.
                   Ideally this would be a text box accepting only valid numbers */
              .label = "TimeMode seconds",
              .settingPtr = &game.settings.timeModeSeconds,
              .type = IntCounter,
              .intMax = TIMEMODESECONDS_MAX,
              .intMin = TIMEMODESECONDS_MIN,
              .intStep = 5,
            },
            {
              .label = "Words per line",
              .settingPtr = &game.settings.wordsPerLine,
              .type = IntCounter,
              .intMax = WORDSPERLINE_MAX,
              .intMin = WORDSPERLINE_MIN,
              .intStep = 1,
            },
            {
              .label = "Font Size",
              .settingPtr = &FONT_SIZE,
              .type = IntSlider,
              .intMax = FONT_SIZE_MAX,
              .intMin = FONT_SIZE_MIN,
              .intStep = 1,
            },
            {
              .label = "Theme",
              .settingPtr = &game.settings.selTheme,
              .type = ThemeSelector,
            },
            {
              .label = "Word List",
              .settingPtr = &game.settings.selWordlist,
              .type = WordlistSelector,
            },
            { .settingPtr = NULL },
        },
        .selSetting = 0,
    };
    int i = 0;
    VisibleSetting *s;
    game.settingsCount = 0;
    while((s = &game.visibleSettings[++i]) && s->settingPtr != NULL) {
        game.settingsCount++;
    }

    init_game(&game);
    parse_args(argc, argv); 

    char config_dir[256];
#if defined(__linux__) /* TODO add cross-platform support */
    snprintf(config_dir, 256, "%s/.config/taip/taip.conf", getenv("HOME"));
#else
    config_dir[0] = 0;
#endif
    parse_config(&game, config_dir);

    reset_game(&game);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "sdl2 initialzation error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow(WINDOW_NAME,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCR_WIDTH, SCR_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Couldn't create SDL_Window: %s\n", SDL_GetError()); 
        return EXIT_FAILURE;
    }

    SDL_SetWindowResizable(window, true);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Couldn't create SDL_Renderer: %s\n", SDL_GetError()); 
        return EXIT_FAILURE;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "sdl_ttf initialzation error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    int displayed_fontsize = FONT_SIZE;
    TTF_Font *font = TTF_OpenFont(game.settings.fontPath, FONT_SIZE);
    /* TTF_SetFontsize refuses to cooperate with me, hence font_small 
       this is TEMPORARY */
    TTF_Font *font_small = TTF_OpenFont(game.settings.fontPath, FONT_SIZE/2);  
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
            case SDL_TEXTINPUT:
                if (game.state == Idle || game.state == InProgress) {
                    handle_text_input(&game, e.text.text);
                }
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    SDL_GetWindowSize(window, &win_w, &win_h);
                }
                break;
            default: {}
            }
        }

        if (displayed_fontsize != FONT_SIZE) {
            /* again, any attempt to use TTF_SetFontSize results in broken rendering */
            font = TTF_OpenFont(game.settings.fontPath, FONT_SIZE);
            font_small = TTF_OpenFont(game.settings.fontPath, FONT_SIZE/2);  
            if (font == NULL || font_small == NULL) {
                fprintf(stderr, "Font error: %s\n", TTF_GetError());
                exit(EXIT_FAILURE);
            }

            displayed_fontsize = FONT_SIZE;
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
        render_game(renderer, &game, font, font_small);
        SDL_RenderPresent(renderer);
 
        /* limit fps */
        Uint64 frame_time = SDL_GetTicks64() - frame_start;
        if (frame_time < FPS_DELTA) {
            SDL_Delay(FPS_DELTA-frame_time);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_CloseFont(font);
    TTF_CloseFont(font_small);
    
    return EXIT_SUCCESS;
}

#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "game.h"
#include "render.h"
#include "logic.h"

static int win_w, win_h;

void render_caret(SDL_Renderer *renderer, game_t *game, int x, int y)
{
    boxRGBA(renderer, x+1, y, x+3, y-FONT_SIZE,
            game->theme->primary.r, game->theme->primary.g, game->theme->primary.b,
            game->theme->primary.a);
}

void render_text_center(SDL_Renderer *renderer, int x_offset, int y_offset,
                        TTF_Font *font, SDL_Color color, SDL_Color bg_color,
                        const char* text)
{
    SDL_Surface *text_surface;

    if(!(text_surface = TTF_RenderText_Shaded(font, text, color, bg_color))) {
        fprintf(stderr, "sdl_ttf error: %s", TTF_GetError());
        exit(1);
    }

    int txt_w, txt_h;
    TTF_SizeText(font, text, &txt_w, &txt_h);
    SDL_Rect text_rect = { .h = txt_h, .w = txt_w,
                           .x = win_w/2-txt_w/2+x_offset,
                           .y = win_h/2-txt_h/2+y_offset};

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void render_timer(SDL_Renderer *renderer, game_t *game, TTF_Font *font)
{
    char timerText[16];
    snprintf(timerText, 16, "%lu", (game->timers.timeEnd - SDL_GetTicks64())/1000+1);

    render_text_center(renderer, 0, -100,
                       font, game->theme->primary, game->theme->bg,
                       timerText);
}


void render_game(SDL_Renderer *renderer, game_t *game, SDL_Window *window,
                 TTF_Font *font, TTF_Font *font_small)
{

    SDL_GetWindowSize(window, &win_w, &win_h);

    switch (game->state) {
    case Idle:
    case InProgress:
        /* render dim text first, so we can draw progress over it later*/
        for (int i = 0; i < TEXT_LINES; ++i) {
            render_text_center(renderer,
                               0, -50+50*i,
                               font, game->theme->dim, game->theme->bg,
                               game->txtBuff[i]);
        }

        int txt_w, txt_h;
        TTF_SizeText(font, game->txtBuff[0], &txt_w, &txt_h);
        if (txt_w % 2 != 0) txt_w++; /* magic to make it divide nicely and prevent 
                                        the letters from displacing because of ints */
        
        if (game->lineProgress == 0) {
            render_caret(renderer, game, win_w/2-txt_w/2, win_h/2-txt_h/2-2);    
        } else {
            char progress_text[256] = { 0 };
            memcpy(progress_text, game->txtBuff[0], game->lineProgress);
            int ptxt_w, ptxt_h;
            TTF_SizeText(font, progress_text, &ptxt_w, &ptxt_h);
            if (ptxt_w % 2 != 0) ptxt_w--;

            render_caret(renderer, game, win_w/2-txt_w/2+ptxt_w,
                         win_h/2-txt_h/2-2);

            if (game->errorIndex == -1) {
                render_text_center(renderer,
                                ceil(((double)(txt_w-ptxt_w))/2)*-1, -50,
                                font, game->theme->primary, game->theme->bg,
                                progress_text);
            } else {
                render_text_center(renderer,
                                (txt_w-ptxt_w)/2*-1, -50,
                                font, game->theme->primary, game->theme->error,
                                progress_text);
            }
        }

        if (game->state == InProgress && game->mode == Time)
            render_timer(renderer, game, font);

        break;
        /* TODO improve this screen with more stats */
    case Results: {
        char errors_text[32];
        snprintf(errors_text, 32, "mistakes: %d", game->stats.errors);
        /* TODO more accurate WPM calculcation, currently it arbitraily divides by 5 */
        /* which is probably quite accurate for the default easy word list.*/
        char wpm_text[32];
        snprintf(wpm_text, 32, "WPM: %.2f",
                 (((float)game->stats.totalCharCount / 5) / game->timeModeSeconds) * 60);
        char accuracy_text[32];
        snprintf(accuracy_text, 32, "accuracy %.2f%%",
                 (1-(float)game->stats.errors/game->stats.totalCharCount)*100);

        render_text_center(renderer,
                               0, -50,
                               font, game->theme->primary, game->theme->bg,
                               wpm_text);

        render_text_center(renderer,
                               0, 0,
                               font_small, game->theme->error, game->theme->bg,
                               errors_text);

        render_text_center(renderer,
                               0, 25,
                               font_small, game->theme->dim, game->theme->bg,
                               accuracy_text);

        /* show help */
        render_text_center(renderer,
                               0, win_h/2-FONT_SIZE,
                               font_small, game->theme->dim, game->theme->bg,
                               "Press alt+r to restart.");


        break;
    }
    default: {};
        
    }
   
}

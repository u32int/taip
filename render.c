#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <assert.h>

#include "game.h"
#include "render.h"
#include "logic.h"

static struct TextFlags {
    bool center;
    bool wrap;
} text_flags = {
    .center = true,
    .wrap = false,
};

void render_caret(SDL_Renderer *renderer, game_t *game, int x, int y)
{
    boxRGBA(renderer, x, y+FONT_SIZE/6, x+3, y-FONT_SIZE+FONT_SIZE/12,
            game->theme->primary.r, game->theme->primary.g, game->theme->primary.b,
            game->theme->primary.a);
}

/* centered by default, see struct TextFlags */
void render_text(SDL_Renderer *renderer, int x, int y,
                 TTF_Font *font, SDL_Color color, SDL_Color bg_color,
                 const char* text)
{
    SDL_Surface *text_surface;
    text_surface = text_flags.wrap ?
        TTF_RenderUTF8_Shaded_Wrapped(font, text, color, bg_color, win_w/2)
        : TTF_RenderUTF8_Shaded(font, text, color, bg_color);
    
    if(!text_surface) {
        fprintf(stderr, "sdl_ttf error: %s", TTF_GetError());
        exit(1);
    }

    int txt_w, txt_h;
    txt_w = text_surface->w;
    txt_h = text_surface->h;

    if (text_flags.center) {
        x += win_w/2-txt_w/2;
        y += win_h/2-txt_h/2;
    }

    SDL_Rect text_rect = { .h = txt_h, .w = txt_w,
                           .x = x, .y = y};

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void render_timer(SDL_Renderer *renderer, game_t *game, TTF_Font *font)
{
    char timerText[16];
    snprintf(timerText, 16, "%lu", (game->timers.timeEnd - SDL_GetTicks64())/1000+1);

    render_text(renderer, 0, (-TEXT_LINES/2-1)*FONT_SIZE,
                       font, game->theme->primary, game->theme->bg,
                       timerText);
}

void render_game(SDL_Renderer *renderer, game_t *game, SDL_Window *window,
                 TTF_Font *font, TTF_Font *font_small)
{
    switch (game->state) {
    case Idle:
        if (game->settings.showHints)
            render_text(renderer,
                        0, win_h/2-FONT_SIZE,
                        font_small, game->theme->dim, game->theme->bg,
                        "Press alt-h for help.");
        // fall through
    case InProgress:
        /* render dim text first, so we can draw progress over it later*/
        for (int i = TEXT_LINES-1; i >= 0; --i) {
            render_text(renderer,
                        0, -FONT_SIZE+FONT_SIZE*i,
                        font, game->theme->dim, game->theme->bg,
                        game->txtBuff[i]);
        }

        if (game->state == InProgress && game->mode == Time)
            render_timer(renderer, game, font);

        int txt_w, txt_h;
        TTF_SizeUTF8(font, game->txtBuff[0], &txt_w, &txt_h);
        if (txt_w % 2 != 0) txt_w++; /* magic to make it divide nicely and prevent 
                                        the letters from displacing because of ints */
        
        if (game->lineProgress == 0) {
            render_caret(renderer, game, win_w/2-txt_w/2, win_h/2-txt_h/2-2);    
        } else {
            char progress_text[256] = { 0 };
            memcpy(progress_text, game->txtBuff[0], game->lineProgress);
            int ptxt_w, ptxt_h;
            TTF_SizeUTF8(font, progress_text, &ptxt_w, &ptxt_h);
            if (ptxt_w % 2 != 0) ptxt_w--;

            render_caret(renderer, game, win_w/2-txt_w/2+ptxt_w,
                         win_h/2-txt_h/2-2);

            if (game->errorIndex == -1) {
                render_text(renderer,
                            ceil(((double)(txt_w-ptxt_w))/2)*-1, -FONT_SIZE,
                            font, game->theme->primary, game->theme->bg,
                            progress_text);
            } else {
                render_text(renderer,
                            (txt_w-ptxt_w)/2*-1, -FONT_SIZE,
                            font, game->theme->primary, game->theme->error,
                            progress_text);
            }
        }

        break;
        /* TODO improve this screen with more stats */
    case Results: {
        char errors_text[32];
        snprintf(errors_text, 32, "mistakes: %d", game->stats.errors);
        /* TODO more accurate WPM calculcation, currently it arbitraily divides by 5 */
        /* which is probably quite accurate for the default easy word list.*/
        char wpm_text[32];
        snprintf(wpm_text, 32, "WPM: %.2f",
                 (((float)game->stats.totalCharCount / 5) / game->settings.timeModeSeconds) * 60);
        char accuracy_text[32];
        snprintf(accuracy_text, 32, "accuracy %.2f%%",
                 (1-(float)game->stats.errors/game->stats.totalCharCount)*100);

        render_text(renderer,
                    0, -50,
                    font, game->theme->primary, game->theme->bg,
                    wpm_text);

        render_text(renderer,
                    0, 0,
                    font_small, game->theme->error, game->theme->bg,
                    errors_text);

        render_text(renderer,
                    0, 25,
                    font_small, game->theme->dim, game->theme->bg,
                    accuracy_text);


        render_text(renderer,
                    0, win_h/2-FONT_SIZE,
                    font_small, game->theme->dim, game->theme->bg,
                    "Press alt+r to restart.");
        break;
    }
    case Help:
        text_flags.wrap = true ;
        text_flags.center = false;
        const char* help_text;
        help_text = "taip is a simple typing game written in C and SDL2.\n\n\
To begin playing, simply start typing!\n\n\
Keybindings: \n\
   Alt-h:    Show this screen\n\
   Alt-r:    Restart game\n\
   Alt-s:    Enter the settings menu\n\
";

        render_text(renderer, win_w/4, FONT_SIZE,
                    font_small, game->theme->primary, game->theme->bg, help_text);

        text_flags.wrap = false;
        text_flags.center = true;

        render_text(renderer,
                    0, win_h/2-FONT_SIZE,
                    font_small, game->theme->dim, game->theme->bg,
                    "Press ESC to go back.");
        break;
    case Settings: {
        char counter_text[16];

        text_flags.center = false;

        int i = 0;
        VisibleSetting curr_set;
        while((curr_set = game->visibleSettings[i++]), curr_set.settingPtr != NULL) {
            SDL_Color draw_color = game->selSetting == i-1 ?
                game->theme->primary : game->theme->dim;

            render_text(renderer,
                            win_w/8, FONT_SIZE*i,
                            font_small, draw_color, game->theme->bg,
                            curr_set.label);

            switch(curr_set.type) {
            case BoolSwitch: {
                render_text(renderer,
                            win_w/2, FONT_SIZE*i,
                            font_small,
                            *(bool*)curr_set.settingPtr ?
                            game->theme->primary : game->theme->dim,
                            game->theme->bg,
                            *(bool*)curr_set.settingPtr ? "True" : "False");
                break;
            }
            case IntSlider: {
                int slider_length = FONT_SIZE*3;
                float slider_progress = ((float)*(int*)curr_set.settingPtr)/curr_set.intMax;

                slider_progress = slider_progress*slider_length;

                boxRGBA(renderer,
                        win_w/2, FONT_SIZE*i,
                        win_w/2+slider_length, FONT_SIZE*(i+1)-FONT_SIZE/2,
                        game->theme->dim.r, game->theme->dim.g, game->theme->dim.b,
                        game->theme->dim.a);

                boxRGBA(renderer,
                        win_w/2, FONT_SIZE*i,
                        win_w/2+(int)slider_progress, FONT_SIZE*(i+1)-FONT_SIZE/2,
                        game->theme->primary.r, game->theme->primary.g, game->theme->primary.b,
                        game->theme->primary.a);

                snprintf(counter_text, 16, "%d", *(int*)curr_set.settingPtr);
                render_text(renderer,
                            win_w/2+slider_length+FONT_SIZE/4, FONT_SIZE*i-1,
                            font_small, game->theme->primary, game->theme->bg,
                            counter_text);

                break;
            case IntCounter:
                snprintf(counter_text, 16, "%d", *(int*)curr_set.settingPtr);
                render_text(renderer,
                            win_w/2, FONT_SIZE*i,
                            font_small, game->theme->primary, game->theme->bg,
                            counter_text);

                if(*(int*)curr_set.settingPtr != curr_set.intMin) {
                    filledTrigonRGBA(renderer,
                                     win_w/2-FONT_SIZE/3, FONT_SIZE*i+FONT_SIZE/3,
                                     win_w/2-FONT_SIZE/8, FONT_SIZE*i+3,
                                     win_w/2-FONT_SIZE/8, FONT_SIZE*i+FONT_SIZE/2,
                                     game->theme->dim.r, game->theme->dim.g,
                                     game->theme->dim.b, game->theme->dim.a);
                }

                if(*(int*)curr_set.settingPtr != curr_set.intMax) {
                    int ttxt_w, ttxt_h;
                    TTF_SizeUTF8(font_small, counter_text, &ttxt_w, &ttxt_h);

                    filledTrigonRGBA(renderer,
                                     win_w/2+FONT_SIZE/3+ttxt_w, FONT_SIZE*i+FONT_SIZE/3,
                                     win_w/2+FONT_SIZE/8+ttxt_w, FONT_SIZE*i+3,
                                     win_w/2+FONT_SIZE/8+ttxt_w, FONT_SIZE*i+FONT_SIZE/2,
                                     game->theme->dim.r, game->theme->dim.g,
                                     game->theme->dim.b, game->theme->dim.a);
                }

                break;
            }
            case ThemeSelector:
                render_text(renderer,
                            win_w/2, FONT_SIZE*i,
                            font_small, game->theme->primary, game->theme->bg,
                            game->theme->prettyName);

                if(game->selTheme != 0) {
                    filledTrigonRGBA(renderer,
                                     win_w/2-FONT_SIZE/3, FONT_SIZE*i+FONT_SIZE/4+3,
                                     win_w/2-FONT_SIZE/12, FONT_SIZE*i+3,
                                     win_w/2-FONT_SIZE/12, FONT_SIZE*i+FONT_SIZE/2+3,
                                     game->theme->dim.r, game->theme->dim.g,
                                     game->theme->dim.b, game->theme->dim.a);
                }

                if(game->selTheme != ThemesCount-1) {
                    int ttxt_w, ttxt_h;
                    TTF_SizeUTF8(font_small, game->theme->prettyName, &ttxt_w, &ttxt_h);

                    filledTrigonRGBA(renderer,
                                     win_w/2+FONT_SIZE/3+ttxt_w, FONT_SIZE*i+FONT_SIZE/4+3,
                                     win_w/2+FONT_SIZE/12+ttxt_w, FONT_SIZE*i+3,
                                     win_w/2+FONT_SIZE/12+ttxt_w, FONT_SIZE*i+FONT_SIZE/2+3,
                                     game->theme->dim.r, game->theme->dim.g,
                                     game->theme->dim.b, game->theme->dim.a);
                }
                break;
            default:
                assert(0 && "Setting type unimplemented");
            }
        }
        
        text_flags.center = true;
                
        render_text(renderer,
                    0, win_h/2-FONT_SIZE,
                    font_small, game->theme->dim, game->theme->bg,
                    "Press ESC to go back.");
        break;
    }
    default: {};
        
    }
   
}

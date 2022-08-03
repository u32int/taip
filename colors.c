#include "colors.h"
#include "game.h"

ThemeColors Colors_Default = {
    .bg      = {18, 18, 18, 255},
    .dim     = {60, 60, 60, 255},
    .error   = {240, 30, 20, 150},
    .primary = {240, 240, 240, 255},
};

ThemeColors Colors_DefaultLight = {
    .bg      = {244, 244, 227, 255},
    .dim     = {160, 160, 160, 255},
    .error   = {240, 30, 20, 255},
    .primary = {22, 22, 22, 255},
};

void set_theme(game_t *game, Theme theme)
{
    switch(theme) {
    case Default:
        game->theme = &Colors_Default;
        break;
    case DefaultLight:
        game->theme = &Colors_DefaultLight;
        break;
    default: {};
    }
}


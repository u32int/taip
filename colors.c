#include "colors.h"

ThemeData Colors_Default = {
    .prettyName = "Default",
    .bg      = {18, 18, 18, 255},
    .dim     = {60, 60, 60, 255},
    .error   = {240, 30, 20, 150},
    .primary = {240, 240, 240, 255},
};

ThemeData Colors_DefaultLight = {
    .prettyName = "Default Light",
    .bg      = {244, 244, 227, 255},
    .dim     = {160, 160, 160, 255},
    .error   = {240, 30, 20, 255},
    .primary = {22, 22, 22, 255},
};

ThemeData Colors_DarkYellow = {
    .prettyName = "Dark-Yellow",
    .bg      = {18, 18, 18, 255},
    .dim     = {60, 60, 60, 255},
    .error   = {240, 30, 20, 150},
    .primary = {240, 220, 35, 255},
};

ThemeData Colors_SolarizedDark = {
    .prettyName = "Solarized Dark",
    .bg      = {0, 43, 54, 255},
    .dim     = {101, 123, 131, 255},
    .error   = {220, 50, 47, 150},
    .primary = {238, 232, 213, 255},
};

void set_theme(game_t *game, Theme theme)
{
    game->selTheme = theme;

    switch(theme) {
    case DefaultLight:
        game->theme = &Colors_DefaultLight;
        break;
    case DarkYellow:
        game->theme = &Colors_DarkYellow;
        break;
    case SolarizedDark:
        game->theme = &Colors_SolarizedDark;
        break;
    default:
        game->theme = &Colors_Default;
    }
}


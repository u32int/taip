#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "config.h"
#include "game.h"
#include "util.h"

int curr_line = 0;

void get_rvalue(const char *line, char *rvalue_buff)
{
    int i = 0;
    while (line[i++] != '=') {
        if (i > 64) {
            fprintf(stderr, "taip: Config parse error\nline %d: %s", curr_line, line);
            exit(1);
        }
    };

    if (line[i] == ' ') /* allow one space around '=' */
        i++;

    strcpy(rvalue_buff, &line[i]);
}

bool parse_opt(const char *optname, const char *line, char *rvalue_buff)
{
    if (!str_starts_with(optname, line))
        return false;

    get_rvalue(line, rvalue_buff);

    return true;
}

void set_bool_from_rvalue(bool *var, char* rvalue)
{
    if (strcmp(rvalue, "true")) {
        *var = true;
    } else if (strcmp(rvalue, "false")) {
        *var = false;
    } else {
        fprintf(stderr, "taip: Config parse error on line %d - invalid rvalue:\n  %s",
                curr_line, rvalue);
        exit(1);               
    }
}

void parse_config(game_t *game, const char *configFilePath)
{
    FILE *file = fopen(configFilePath, "r");

    if (file == NULL) {
        return;
    }

    char *buff = (char*)malloc(64);
    size_t buff_size = 64;

    char *rvalue_buff = (char*)calloc(64, sizeof(char));

    while (getline(&buff, &buff_size, file) != -1) {
        curr_line++;
        if (buff[0] == '#' || buff[0] == '\n') {
            /* Line is a comment or empty, ignore */
        } else if (parse_opt("timeModeSeconds", buff, rvalue_buff)) {
            game->settings.timeModeSeconds = clamp_int(strtoimax(rvalue_buff, NULL, 10),
                                                       TIMEMODESECONDS_MIN,
                                                       TIMEMODESECONDS_MAX);
        } else if (parse_opt("fontSize", buff, rvalue_buff)) {
            FONT_SIZE = clamp_int(strtoimax(rvalue_buff, NULL, 10),
                                  FONT_SIZE_MIN, FONT_SIZE_MAX);
        } else if (parse_opt("wordsPerLine", buff, rvalue_buff)) {
            game->settings.wordsPerLine = clamp_int(strtoimax(rvalue_buff, NULL, 10),
                                                    WORDSPERLINE_MIN, WORDSPERLINE_MAX);
        } else if (parse_opt("showHints", buff, rvalue_buff)) {
            set_bool_from_rvalue(&game->settings.showHints, rvalue_buff);
        } else if (parse_opt("focusMode", buff, rvalue_buff)) {
            set_bool_from_rvalue(&game->settings.focusMode, rvalue_buff);
        } else {
            fprintf(stderr, "taip: Config parse error - invalid line/option\nline %d: %s",
                    curr_line, buff);
            exit(1);
        }
    }

    free(buff);
    free(rvalue_buff);
    fclose(file);
}

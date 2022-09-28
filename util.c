#include <stdio.h>
#include <string.h>

#include "util.h"

int clamp_int(int a, int min, int max)
{
    if (a > max) return max;
    return a < min ? min : a;
}


bool str_starts_with(const char* substr, const char* str)
{
    const size_t str_len = strlen(str);
    const size_t substr_len = strlen(substr);

    if (str_len < substr_len)
        return false;

    for (size_t i = 0; i < str_len; ++i) {
        if (str[i] != substr[i]) {
            return false;
        }
    }

    return true;
}

bool str_ends_with(const char* substr, const char* str)
{
    const size_t str_len = strlen(str);
    const size_t substr_len = strlen(substr);

    if (str_len < substr_len)
        return false;

    for (size_t i = 0; i < substr_len; ++i) {
        if (str[str_len-1-i] != substr[substr_len-1-i]) {
            return false;
        }
    }

    return true;
}


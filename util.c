
#include "util.h"
#include <stdio.h>
#include <string.h>

int clamp_int(int a, int min, int max)
{
    if (a > max) return max;
    return a < min ? min : a;
}


bool str_starts_with(const char* substr, const char* str)
{
    for (int i = 0; i < strlen(substr); ++i) {
        if (str[i] != substr[i]) {
            return false;
        }
    }
    return true;
}

bool str_ends_with(const char* substr, const char* str)
{
    for (int i = 0; i < strlen(substr); ++i) {
        if (str[strlen(str)-1-i] != substr[strlen(substr)-1-i]) {
            return false;
        }
    }
    return true;
}


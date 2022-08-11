#include "util.h"

int clamp_int(int a, int min, int max)
{
    if (a > max) return max;
    return a < min ? min : a;
}

#pragma once

#include <stdbool.h>

int clamp_int(int a, int min, int max);

bool str_starts_with(const char* substr, const char* str);
bool str_ends_with(const char* substr, const char* str);

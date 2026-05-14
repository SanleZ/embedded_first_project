#pragma once

#include <stdbool.h>
#include <stdint.h>

#define STR_NULL ((void *)0)

int str_compare(const char *a, const char *b);

char *str_tokenize(char *str, char **context);

bool str_to_u32(const char *str, uint32_t *value);

void str_concat(char *str, const char *add, uint32_t max_len);

uint32_t str_len(const char *s);

char *str_u32_to_str(int32_t val, char *buf);

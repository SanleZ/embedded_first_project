#include "common/string_utils.h"
#include "common/common.h"
#include <stdbool.h>
#include <stdint.h>

static bool cli_is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

char *str_tokenize(char *str, char **context) {
  char *start;

  if (str != COMMON_NULL) {
    *context = str;
  }

  if (*context == COMMON_NULL) {
    return COMMON_NULL;
  }

  while (**context && cli_is_space(**context)) {
    (*context)++;
  }

  if (**context == '\0') {
    return COMMON_NULL;
  }

  start = *context;

  while (**context && !cli_is_space(**context)) {
    (*context)++;
  }

  if (**context) {
    **context = '\0';
    (*context)++;
  }

  return start;
}

bool str_to_u32(const char *str, uint32_t *value) {
  uint32_t result = 0;

  if (str == COMMON_NULL || value == COMMON_NULL) {
    return false;
  }

  if (*str == '\0') {
    return false;
  }

  while (*str) {

    if (*str < '0' || *str > '9') {
      return false;
    }

    result = (result * 10) + (*str - '0');

    str++;
  }

  *value = result;

  return true;
}

int str_compare(const char *a, const char *b) {
  while (*a && (*a == *b)) {
    a++;
    b++;
  }

  return (uint8_t)*a - (uint8_t)*b;
}

void str_concat(char *str, const char *add, uint32_t max_len) {
  while (*str && max_len > 1) {
    str++;
    max_len--;
  }

  while (*add && max_len > 1) {
    *str = *add;
    str++;
    add++;
    max_len--;
  }
  *str = '\0';
}

uint32_t str_len(const char *s) {
  int len = 0;
  while (*s++) {
    len++;
  }
  return len;
}

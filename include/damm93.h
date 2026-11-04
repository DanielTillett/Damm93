/*
 Base93 (visible ASCII) implementation of the Damm error detection
 algorithm in C

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef DAMM93_H
#define DAMM93_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DAMM93_BASE 93

/* Returned by damm93_check_digit() when the input contains a character
   outside the base93 alphabet. '\0' can never be a check digit. */
#define DAMM93_INVALID '\0'

/*
 The base93 alphabet: every visible ASCII character except backslash, ordered
 so that base93 zero is '0' and the digits keep their values: '0' to '~'
 without backslash, then '!' to '/'. Space and backslash are excluded, so a
 check digit is never invisible or an escape character. Input is
 case-sensitive.
 */
extern const char damm93_alphabet[DAMM93_BASE + 1];

/*
 Returns the value (0-92) of a base93 character, or -1 if the character is not
 in the alphabet.
 */
int damm93_value(char c);

/*
 Returns the base93 check digit for a null-terminated string, or
 DAMM93_INVALID if the string contains a character outside the alphabet.
 */
char damm93_check_digit(const char *code);

/*
 Returns true if the last character of a null-terminated string is a valid
 Damm check digit for the characters before it.
 */
bool damm93_is_valid(const char *code);

#ifdef __cplusplus
}
#endif

#endif /* DAMM93_H */

/*
 Base95 (printable ASCII) implementation of the Damm error detection
 algorithm in C

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef DAMM95_H
#define DAMM95_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DAMM95_BASE 95

/* Returned by damm95_check_digit() when the input contains a character
   outside the base95 alphabet. '\0' can never be a check digit. */
#define DAMM95_INVALID '\0'

/*
 The base95 alphabet: every printable ASCII character, space (32) to '~' (126),
 ordered so that base95 zero is '0' and the digits keep their values:
 '0' to '~', then space to '/'. Input is case-sensitive.
 */
extern const char damm95_alphabet[DAMM95_BASE + 1];

/*
 Returns the value (0-94) of a base95 character, or -1 if the character is not
 printable ASCII.
 */
int damm95_value(char c);

/*
 Returns the base95 check digit for a null-terminated string, or
 DAMM95_INVALID if the string contains a character outside the alphabet.
 */
char damm95_check_digit(const char *code);

/*
 Returns true if the last character of a null-terminated string is a valid
 Damm check digit for the characters before it.
 */
bool damm95_is_valid(const char *code);

#ifdef __cplusplus
}
#endif

#endif /* DAMM95_H */

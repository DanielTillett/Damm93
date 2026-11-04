/*
 Base93 (printable ASCII) implementation of the Damm error detection
 algorithm in C

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause

 Background
 The Damm algorithm is a check digit algorithm that detects all single-digit
 errors and all adjacent transposition errors. Prepending leading zeros
 ('0' in this alphabet) does not affect the check digit.
 https://en.wikipedia.org/wiki/Damm_algorithm
 */

#include "damm93.h"

/* Printable ASCII from '0' to '~' without backslash, then '!' to '/'. */
const char damm93_alphabet[DAMM93_BASE + 1] =
    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    "!\"#$%&'()*+,-./";

/*
 The quasigroup operation x * y = 2(x - y) mod 93. Subtraction gives a zero
 diagonal, and because 2 and 2^2 - 2 are both invertible modulo the odd base
 93 the operation is totally anti-symmetric, so every single-character error
 and adjacent transposition is detected.
 */
static int damm93_op(int x, int y) {
    return 2 * (x - y + DAMM93_BASE) % DAMM93_BASE;
}

int damm93_value(char c) {
    if (c <= ' ' || c > '~' || c == '\\') {
        return -1;
    }
    if (c < '0') {
        return c - '!' + 78; /* '!' to '/' follow the 78 characters '0' to '~' */
    }
    return c < '\\' ? c - '0' : c - '0' - 1;
}

char damm93_check_digit(const char *code) {
    int interim = 0;

    for (const char *p = code; *p != '\0'; ++p) {
        int i = damm93_value(*p);

        if (i < 0) {
            return DAMM93_INVALID;
        }

        interim = damm93_op(interim, i);
    }

    return damm93_alphabet[interim];
}

bool damm93_is_valid(const char *code) {
    return code[0] != '\0' && damm93_check_digit(code) == damm93_alphabet[0];
}

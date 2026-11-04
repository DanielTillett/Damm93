/*
 Base95 (printable ASCII) implementation of the Damm error detection
 algorithm in C

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause

 Background
 The Damm algorithm is a check digit algorithm that detects all single-digit
 errors and all adjacent transposition errors. Prepending leading zeros
 ('0' in this alphabet) does not affect the check digit.
 https://en.wikipedia.org/wiki/Damm_algorithm
 */

#include "damm95.h"

/* Printable ASCII from '0' to '~', then space to '/'. */
const char damm95_alphabet[DAMM95_BASE + 1] =
    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    " !\"#$%&'()*+,-./";

/*
 The quasigroup operation x * y = 2(x - y) mod 95. Subtraction gives a zero
 diagonal, and because 2 and 2^2 - 2 are both invertible modulo the odd base
 95 the operation is totally anti-symmetric, so every single-character error
 and adjacent transposition is detected.
 */
static int damm95_op(int x, int y) {
    return 2 * (x - y + DAMM95_BASE) % DAMM95_BASE;
}

int damm95_value(char c) {
    if (c < ' ' || c > '~') {
        return -1;
    }
    return (c - '0' + DAMM95_BASE) % DAMM95_BASE;
}

char damm95_check_digit(const char *code) {
    int interim = 0;

    for (const char *p = code; *p != '\0'; ++p) {
        int i = damm95_value(*p);

        if (i < 0) {
            return DAMM95_INVALID;
        }

        interim = damm95_op(interim, i);
    }

    return damm95_alphabet[interim];
}

bool damm95_is_valid(const char *code) {
    return code[0] != '\0' && damm95_check_digit(code) == damm95_alphabet[0];
}

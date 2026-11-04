/*
 Unit tests for the base95 Damm check digit library.

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

#include "damm95.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__,   \
                    #cond);                                                    \
            failures++;                                                        \
        }                                                                      \
    } while (0)

/* Small deterministic PRNG so the tests behave identically everywhere. */
static unsigned long rng_state = 20261104UL;

static int rng_next(int bound) {
    rng_state = rng_state * 1103515245UL + 12345UL;
    return (int)((rng_state >> 16) % (unsigned long)bound);
}

static void random_code(char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        buf[i] = damm95_alphabet[rng_next(DAMM95_BASE)];
    }
    buf[len] = '\0';
}

static void append_check_digit(char *buf) {
    size_t len = strlen(buf);
    buf[len] = damm95_check_digit(buf);
    buf[len + 1] = '\0';
}

static void test_alphabet_is_a_bijection(void) {
    CHECK(strlen(damm95_alphabet) == DAMM95_BASE);

    for (int i = 0; i < DAMM95_BASE; i++) {
        CHECK(damm95_value(damm95_alphabet[i]) == i);
    }

    for (int c = 1; c < 256; c++) {
        if (strchr(damm95_alphabet, c) == NULL) {
            CHECK(damm95_value((char)c) == -1);
        }
    }
}

/* Changing either character of any two-character code changes the check
   digit, so every single substitution is detected. */
static void test_every_substitution_is_detected(void) {
    for (int a = 0; a < DAMM95_BASE; a++) {
        bool row[DAMM95_BASE] = {false}, column[DAMM95_BASE] = {false};
        for (int b = 0; b < DAMM95_BASE; b++) {
            char ab[3] = {damm95_alphabet[a], damm95_alphabet[b], '\0'};
            char ba[3] = {damm95_alphabet[b], damm95_alphabet[a], '\0'};
            int r = damm95_value(damm95_check_digit(ab));
            int c = damm95_value(damm95_check_digit(ba));
            CHECK(r >= 0 && !row[r]);
            CHECK(c >= 0 && !column[c]);
            if (r >= 0) {
                row[r] = true;
            }
            if (c >= 0) {
                column[c] = true;
            }
        }
    }
}

/* Exhaustive over every prefix state and every pair of distinct characters. */
static void test_every_adjacent_transposition_is_detected(void) {
    for (int a = 0; a < DAMM95_BASE; a++) {
        for (int x = 0; x < DAMM95_BASE; x++) {
            for (int y = 0; y < DAMM95_BASE; y++) {
                if (x == y) {
                    continue;
                }
                char xy[4] = {damm95_alphabet[a], damm95_alphabet[x], damm95_alphabet[y], '\0'};
                char yx[4] = {damm95_alphabet[a], damm95_alphabet[y], damm95_alphabet[x], '\0'};
                CHECK(damm95_check_digit(xy) != damm95_check_digit(yx));
            }
        }
    }
}

static void test_round_trip_and_leading_zeros(void) {
    for (int n = 0; n < 2000; n++) {
        char code[40], padded[48];
        random_code(code, (size_t)(1 + rng_next(30)));
        snprintf(padded, sizeof padded, "%c%c%c%s", damm95_alphabet[0],
                 damm95_alphabet[0], damm95_alphabet[0], code);
        CHECK(damm95_check_digit(code) == damm95_check_digit(padded));
        append_check_digit(code);
        CHECK(damm95_is_valid(code));
    }
}

static void test_case_sensitive_and_invalid_input(void) {
    CHECK(damm95_check_digit("abc") != damm95_check_digit("ABC"));
    CHECK(damm95_check_digit("ab\tc") == DAMM95_INVALID);
    CHECK(damm95_check_digit("ab\x7f") == DAMM95_INVALID);
    CHECK(damm95_check_digit("Hello world") == '\\');
    CHECK(damm95_check_digit("caf\xc3\xa9") == DAMM95_INVALID);
    CHECK(!damm95_is_valid("ab\tc"));
    CHECK(!damm95_is_valid(""));
    CHECK(damm95_check_digit("") == damm95_alphabet[0]);
}

/* Checks the shared vectors that the PHP and JavaScript tests also use. Each
   line is <code> TAB <check digit>; an empty check digit means invalid, and
   lines without a TAB are comments. */
static void test_shared_vectors(const char *path) {
    FILE *f = fopen(path, "r");
    char line[256];
    int count = 0;

    CHECK(f != NULL);
    if (f == NULL) {
        return;
    }

    while (fgets(line, sizeof line, f) != NULL) {
        char *tab = strchr(line, '\t');

        if (tab == NULL) {
            continue;
        }

        *tab = '\0';
        char expected = (tab[1] == '\n' || tab[1] == '\0') ? DAMM95_INVALID : tab[1];
        if (damm95_check_digit(line) != expected) {
            fprintf(stderr, "vector '%s': got '%c', expected '%c'\n", line,
                    damm95_check_digit(line), expected);
            failures++;
        }
        count++;
    }

    fclose(f);
    CHECK(count > 1000);
}

int main(int argc, char **argv) {
    test_alphabet_is_a_bijection();
    test_every_substitution_is_detected();
    test_every_adjacent_transposition_is_detected();
    test_round_trip_and_leading_zeros();
    test_case_sensitive_and_invalid_input();

    if (argc > 1) {
        test_shared_vectors(argv[1]);
    }

    if (failures != 0) {
        fprintf(stderr, "%d check(s) failed\n", failures);
        return EXIT_FAILURE;
    }

    puts("All tests passed");
    return EXIT_SUCCESS;
}

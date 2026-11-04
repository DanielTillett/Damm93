/*
 Unit tests for the base93 Damm check digit library.

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

#include "damm93.h"

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
        buf[i] = damm93_alphabet[rng_next(DAMM93_BASE)];
    }
    buf[len] = '\0';
}

static void append_check_digit(char *buf) {
    size_t len = strlen(buf);
    buf[len] = damm93_check_digit(buf);
    buf[len + 1] = '\0';
}

static void test_alphabet_is_a_bijection(void) {
    CHECK(strlen(damm93_alphabet) == DAMM93_BASE);

    for (int i = 0; i < DAMM93_BASE; i++) {
        CHECK(damm93_value(damm93_alphabet[i]) == i);
    }

    for (int c = 1; c < 256; c++) {
        if (strchr(damm93_alphabet, c) == NULL) {
            CHECK(damm93_value((char)c) == -1);
        }
    }
}

/* Changing either character of any two-character code changes the check
   digit, so every single substitution is detected. */
static void test_every_substitution_is_detected(void) {
    for (int a = 0; a < DAMM93_BASE; a++) {
        bool row[DAMM93_BASE] = {false}, column[DAMM93_BASE] = {false};
        for (int b = 0; b < DAMM93_BASE; b++) {
            char ab[3] = {damm93_alphabet[a], damm93_alphabet[b], '\0'};
            char ba[3] = {damm93_alphabet[b], damm93_alphabet[a], '\0'};
            int r = damm93_value(damm93_check_digit(ab));
            int c = damm93_value(damm93_check_digit(ba));
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
    for (int a = 0; a < DAMM93_BASE; a++) {
        for (int x = 0; x < DAMM93_BASE; x++) {
            for (int y = 0; y < DAMM93_BASE; y++) {
                if (x == y) {
                    continue;
                }
                char xy[4] = {damm93_alphabet[a], damm93_alphabet[x], damm93_alphabet[y], '\0'};
                char yx[4] = {damm93_alphabet[a], damm93_alphabet[y], damm93_alphabet[x], '\0'};
                CHECK(damm93_check_digit(xy) != damm93_check_digit(yx));
            }
        }
    }
}

static void test_round_trip_and_leading_zeros(void) {
    for (int n = 0; n < 2000; n++) {
        char code[40], padded[48];
        random_code(code, (size_t)(1 + rng_next(30)));
        snprintf(padded, sizeof padded, "%c%c%c%s", damm93_alphabet[0],
                 damm93_alphabet[0], damm93_alphabet[0], code);
        CHECK(damm93_check_digit(code) == damm93_check_digit(padded));
        append_check_digit(code);
        CHECK(damm93_is_valid(code));
    }
}

static void test_case_sensitive_and_invalid_input(void) {
    CHECK(damm93_check_digit("abc") != damm93_check_digit("ABC"));
    CHECK(damm93_check_digit("ab\tc") == DAMM93_INVALID);
    CHECK(damm93_check_digit("ab\x7f") == DAMM93_INVALID);
    CHECK(damm93_check_digit("Hello_world") == 'u');
    CHECK(damm93_check_digit("ab c") == DAMM93_INVALID);
    CHECK(damm93_check_digit("a\\b") == DAMM93_INVALID);
    CHECK(damm93_check_digit("caf\xc3\xa9") == DAMM93_INVALID);
    CHECK(!damm93_is_valid("ab\tc"));
    CHECK(!damm93_is_valid(""));
    CHECK(damm93_check_digit("") == damm93_alphabet[0]);
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
        char expected = (tab[1] == '\n' || tab[1] == '\0') ? DAMM93_INVALID : tab[1];
        if (damm93_check_digit(line) != expected) {
            fprintf(stderr, "vector '%s': got '%c', expected '%c'\n", line,
                    damm93_check_digit(line), expected);
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

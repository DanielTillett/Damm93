/*
 Command-line interface for the base95 Damm check digit library.

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L /* fileno, isatty */
#endif

#include "damm95.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

#ifndef DAMM95_VERSION
#define DAMM95_VERSION "1.0.0"
#endif

enum mode { MODE_REPORT, MODE_APPEND, MODE_CHECK };

enum exit_status { EXIT_OK = 0, EXIT_INVALID = 1, EXIT_USAGE = 2 };

static void usage(FILE *out) {
    fputs("Usage: damm95 [-a | -c] [code ...]\n"
          "\n"
          "Calculate or verify base95 Damm check digits. With no codes, codes\n"
          "are read from standard input, one per line. Use -- before a code\n"
          "that starts with '-'.\n"
          "\n"
          "  -a, --append   print each code with its check digit appended\n"
          "  -c, --check    print nothing; exit 0 only if every code is valid\n"
          "  -h, --help     show this help\n"
          "  -V, --version  show the version\n"
          "\n"
          "Alphabet: printable ASCII, space to ~ (case-sensitive). Quote codes in the shell.\n"
          "Exit status: 0 success, 1 invalid code or character, 2 usage error\n",
          out);
}

/* Processes one code and returns true if it passed for the selected mode. */
static bool process(const char *code, enum mode mode) {
    char digit = damm95_check_digit(code);

    switch (mode) {
    case MODE_APPEND:
        if (digit == DAMM95_INVALID) {
            fprintf(stderr, "damm95: invalid character in '%s'\n", code);
            return false;
        }
        printf("%s%c\n", code, digit);
        return true;

    case MODE_CHECK:
        return damm95_is_valid(code);

    case MODE_REPORT:
    default:
        if (digit == DAMM95_INVALID) {
            printf("Base95 Number: %s\nBase95 Check Digit: invalid character\nError Check: 0\n", code);
        } else {
            printf("Base95 Number: %s\nBase95 Check Digit: %c\nError Check: %d\n",
                   code, digit, damm95_is_valid(code) ? 1 : 0);
        }
        return digit != DAMM95_INVALID;
    }
}

/* Reads the next line from a stream without its line ending, or NULL at end of
   input. The caller frees it. */
static char *read_line(FILE *in) {
    size_t len = 0, cap = 64;
    char *buf = malloc(cap);
    int c;

    if (buf == NULL) {
        return NULL;
    }

    while ((c = getc(in)) != EOF && c != '\n') {
        if (len + 1 == cap) {
            char *grown = realloc(buf, cap *= 2);
            if (grown == NULL) {
                free(buf);
                return NULL;
            }
            buf = grown;
        }
        buf[len++] = (char)c;
    }

    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }

    if (len > 0 && buf[len - 1] == '\r') {
        len--;
    }

    buf[len] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    enum mode mode = MODE_REPORT;
    int first = 1;
    bool ok = true;

    for (; first < argc && argv[first][0] == '-' && argv[first][1] != '\0'; first++) {
        const char *arg = argv[first];

        if (strcmp(arg, "--") == 0) {
            first++;
            break;
        } else if (strcmp(arg, "-a") == 0 || strcmp(arg, "--append") == 0) {
            mode = MODE_APPEND;
        } else if (strcmp(arg, "-c") == 0 || strcmp(arg, "--check") == 0) {
            mode = MODE_CHECK;
        } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            usage(stdout);
            return EXIT_OK;
        } else if (strcmp(arg, "-V") == 0 || strcmp(arg, "--version") == 0) {
            puts("damm95 " DAMM95_VERSION);
            return EXIT_OK;
        } else {
            fprintf(stderr, "damm95: unknown option '%s'\n", arg);
            usage(stderr);
            return EXIT_USAGE;
        }
    }

    if (first < argc) {
        for (int i = first; i < argc; i++) {
            ok = process(argv[i], mode) && ok;
        }
    } else if (isatty(fileno(stdin))) {
        usage(stderr);
        return EXIT_USAGE;
    } else {
        char *code;
        bool any = false;

        while ((code = read_line(stdin)) != NULL) {
            if (code[0] == '\0') {
                free(code);
                continue;
            }
            any = true;
            ok = process(code, mode) && ok;
            free(code);
        }

        if (!any) {
            usage(stderr);
            return EXIT_USAGE;
        }
    }

    return ok ? EXIT_OK : EXIT_INVALID;
}

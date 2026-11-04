#!/usr/bin/env node

/*
 Command-line interface for the base93 Damm check digit library.
 Behaves the same as the C version.

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

import { readFileSync } from 'node:fs';
import { INVALID, checkDigit, isValid } from '../damm93.js';

const VERSION = '1.0.0';
const EXIT_OK = 0;
const EXIT_INVALID = 1;
const EXIT_USAGE = 2;

const USAGE = `Usage: damm93 [-a | -c] [code ...]

Calculate or verify base93 Damm check digits. With no codes, codes
are read from standard input, one per line. Use -- before a code
that starts with '-'.

  -a, --append   print each code with its check digit appended
  -c, --check    print nothing; exit 0 only if every code is valid
  -h, --help     show this help
  -V, --version  show the version

Alphabet: visible ASCII except backslash (case-sensitive). Quote codes in the shell.
Exit status: 0 success, 1 invalid code or character, 2 usage error
`;

/* Processes one code and returns true if it passed for the selected mode. */
function processCode(code, mode) {
  const digit = checkDigit(code);

  switch (mode) {
    case 'append':
      if (digit === INVALID) {
        process.stderr.write(`damm93: invalid character in '${code}'\n`);
        return false;
      }
      process.stdout.write(`${code}${digit}\n`);
      return true;

    case 'check':
      return isValid(code);

    default:
      process.stdout.write(
        `Base93 Number: ${code}\nBase93 Check Digit: ${digit ?? 'invalid character'}\nError Check: ${isValid(code) ? 1 : 0}\n`,
      );
      return digit !== INVALID;
  }
}

function main(argv) {
  let mode = 'report';
  const args = [...argv];

  while (args.length > 0 && args[0].length > 1 && args[0][0] === '-') {
    const arg = args.shift();

    if (arg === '--') {
      break;
    } else if (arg === '-a' || arg === '--append') {
      mode = 'append';
    } else if (arg === '-c' || arg === '--check') {
      mode = 'check';
    } else if (arg === '-h' || arg === '--help') {
      process.stdout.write(USAGE);
      return EXIT_OK;
    } else if (arg === '-V' || arg === '--version') {
      process.stdout.write(`damm93 ${VERSION}\n`);
      return EXIT_OK;
    } else {
      process.stderr.write(`damm93: unknown option '${arg}'\n${USAGE}`);
      return EXIT_USAGE;
    }
  }

  let codes = args;

  if (codes.length === 0) {
    if (process.stdin.isTTY) {
      process.stderr.write(USAGE);
      return EXIT_USAGE;
    }
    codes = readFileSync(0, 'utf8').split(/\r?\n/).filter(Boolean);
    if (codes.length === 0) {
      process.stderr.write(USAGE);
      return EXIT_USAGE;
    }
  }

  let ok = true;
  for (const code of codes) {
    ok = processCode(code, mode) && ok;
  }

  return ok ? EXIT_OK : EXIT_INVALID;
}

process.exitCode = main(process.argv.slice(2));

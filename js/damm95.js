/*
 Base95 (printable ASCII) implementation of the Damm error detection
 algorithm in JavaScript

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

/**
 * Every printable ASCII character, space to '~', ordered so that base95 zero
 * is '0' and the digits keep their values. Input is case-sensitive.
 */
export const ALPHABET =
  '0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~' + ' !"#$%&\'()*+,-./';

/** Returned by checkDigit() when the input contains a character outside the alphabet. */
export const INVALID = null;

const BASE = 95;

/*
 The quasigroup operation x * y = 2(x - y) mod 95. It has a zero diagonal and
 is totally anti-symmetric, so every single-character error and adjacent
 transposition is detected.
 */
function op(x, y) {
  return (2 * (x - y + BASE)) % BASE;
}

const VALUES = new Map([...ALPHABET].map((char, i) => [char, i]));

/**
 * Returns the value (0-94) of a base95 character, or -1 if it is not printable ASCII.
 * @param {string} char
 * @returns {number}
 */
export function value(char) {
  return VALUES.get(char) ?? -1;
}

/**
 * Returns the base95 check digit for a string, or INVALID (null) if the string
 * contains a character outside the alphabet.
 * @param {string} code
 * @returns {string | null}
 */
export function checkDigit(code) {
  if (typeof code !== 'string') {
    throw new TypeError('code must be a string');
  }

  let interim = 0;

  for (const char of code) {
    const v = value(char);

    if (v < 0) {
      return INVALID;
    }

    interim = op(interim, v);
  }

  return ALPHABET[interim];
}

/**
 * Returns true if the last character of a string is a valid Damm check digit
 * for the characters before it.
 * @param {string} code
 * @returns {boolean}
 */
export function isValid(code) {
  return code !== '' && checkDigit(code) === ALPHABET[0];
}

/*
 Tests for the JavaScript base93 Damm check digit library. Run with `npm test`.

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { test } from 'node:test';
import { fileURLToPath } from 'node:url';
import { ALPHABET, INVALID, checkDigit, isValid, value } from './damm93.js';

const VECTORS = fileURLToPath(new URL('../tests/vectors.txt', import.meta.url));
const CLI = fileURLToPath(new URL('./bin/damm93.js', import.meta.url));
const CHARS = [...ALPHABET];

test('alphabet is a bijection', () => {
  assert.equal(new Set(CHARS).size, 93);
  CHARS.forEach((char, i) => assert.equal(value(char), i));
  for (const char of '\t\n\u007fé€') {
    assert.equal(value(char), -1, `'${char}' should not be in the alphabet`);
  }
});

test('every single substitution and adjacent transposition is detected', () => {
  for (const a of CHARS) {
    assert.equal(new Set(CHARS.map((b) => checkDigit(a + b))).size, 93, `row ${a}`);
    assert.equal(new Set(CHARS.map((b) => checkDigit(b + a))).size, 93, `column ${a}`);

    for (const x of CHARS) {
      for (const y of CHARS) {
        if (x !== y && checkDigit(a + x + y) === checkDigit(a + y + x)) {
          assert.fail(`transposition ${a}${x}${y} / ${a}${y}${x} not detected`);
        }
      }
    }
  }
});

test('known values, case-sensitivity and invalid input', () => {
  assert.equal(checkDigit('World'), 'q');
  assert.ok(isValid('Worldq'));
  assert.ok(!isValid('Wordlq'));
  assert.equal(checkDigit('Hello_world'), 'u');
  assert.equal(checkDigit('ab c'), INVALID);
  assert.equal(checkDigit('a\\b'), INVALID);
  assert.notEqual(checkDigit('abc'), checkDigit('ABC'));
  assert.equal(checkDigit('ab\tc'), INVALID);
  assert.equal(checkDigit('café'), INVALID);
  assert.ok(!isValid(''));
  assert.throws(() => checkDigit(42), TypeError);
});

test('shared vectors match the reference implementation', () => {
  let count = 0;
  for (const line of readFileSync(VECTORS, 'utf8').split('\n')) {
    if (!line.includes('\t')) {
      continue;
    }
    const [code, expected] = line.split('\t');
    assert.equal(checkDigit(code), expected === '' ? INVALID : expected, `vector ${code}`);
    count++;
  }
  assert.ok(count > 1000);
});

test('CLI matches the C version', () => {
  const run = (args, input) => spawnSync(process.execPath, [CLI, ...args], { input, encoding: 'utf8' });

  assert.equal(run(['World']).stdout, 'Base93 Number: World\nBase93 Check Digit: q\nError Check: 0\n');
  assert.equal(
    run(['café']).stdout,
    'Base93 Number: café\nBase93 Check Digit: invalid character\nError Check: 0\n',
  );
  assert.equal(run(['-a', 'World']).stdout, 'Worldq\n');
  assert.equal(run(['-a'], 'World\n-dash\nHello_world\n\n').stdout, 'Worldq\n-dash:\nHello_worldu\n');
  assert.equal(run(['-a', '--', '-dash']).stdout, '-dash:\n');
  assert.equal(run(['-c', 'Worldq']).status, 0);
  assert.equal(run(['-c', 'Wordlq']).status, 1);
  assert.equal(run(['-a', 'café']).status, 1);
  assert.equal(run(['--bogus']).status, 2);
});

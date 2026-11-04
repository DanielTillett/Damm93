<?php

/*
 Tests for the PHP base93 Damm check digit library. Dependency-free: run with
 `php php/tests/Damm93Test.php`.

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

declare(strict_types=1);

require_once __DIR__ . '/../src/Damm93.php';

use Damm93\Damm93;

$failures = 0;

function check(bool $condition, string $message): void
{
    global $failures;
    if (!$condition) {
        fwrite(STDERR, "FAIL: $message\n");
        $failures++;
    }
}

$alphabet = str_split(Damm93::ALPHABET);

/* Alphabet is a bijection. */
check(count(array_unique($alphabet)) === Damm93::BASE, 'alphabet has 93 distinct characters');
foreach ($alphabet as $i => $char) {
    check(Damm93::value($char) === $i, "value of '$char'");
}
for ($c = 0; $c < 256; $c++) {
    if (strpos(Damm93::ALPHABET, chr($c)) === false) {
        check(Damm93::value(chr($c)) === -1, "byte $c is not in the alphabet");
    }
}

/* Every single substitution and adjacent transposition is detected (exhaustive). */
foreach ($alphabet as $a) {
    $row = [];
    $column = [];
    foreach ($alphabet as $b) {
        $row[] = Damm93::checkDigit($a . $b);
        $column[] = Damm93::checkDigit($b . $a);
    }
    check(count(array_unique($row)) === Damm93::BASE, "row $a is a permutation");
    check(count(array_unique($column)) === Damm93::BASE, "column $a is a permutation");

    foreach ($alphabet as $x) {
        foreach ($alphabet as $y) {
            if ($x !== $y && Damm93::checkDigit($a . $x . $y) === Damm93::checkDigit($a . $y . $x)) {
                check(false, "transposition $a$x$y / $a$y$x not detected");
            }
        }
    }
}

/* Case-sensitivity and invalid input. */
check(Damm93::checkDigit('abc') !== Damm93::checkDigit('ABC'), 'input is case-sensitive');
check(Damm93::checkDigit("ab\tc") === Damm93::INVALID, 'tab is invalid');
check(Damm93::checkDigit('Hello_world') === 'u', 'Hello_world -> u');
check(Damm93::checkDigit('ab c') === Damm93::INVALID, 'space is invalid');
check(Damm93::checkDigit('a\\b') === Damm93::INVALID, 'backslash is invalid');
check(Damm93::checkDigit('café') === Damm93::INVALID, 'non-ASCII is invalid');
check(Damm93::checkDigit('World') === 'q', 'World -> q');
check(Damm93::isValid('Worldq'), 'Worldq is valid');
check(!Damm93::isValid(''), 'empty string is invalid');

/* Shared vectors, identical to those used by the C and JavaScript tests. */
$count = 0;
foreach (file(__DIR__ . '/../../tests/vectors.txt', FILE_IGNORE_NEW_LINES) as $line) {
    if (strpos($line, "\t") === false) {
        continue;
    }
    [$code, $expected] = explode("\t", $line, 2);
    $expected = $expected === '' ? Damm93::INVALID : $expected;
    check(Damm93::checkDigit($code) === $expected, "vector $code");
    $count++;
}
check($count > 1000, 'shared vectors were read');

if ($failures > 0) {
    fwrite(STDERR, "$failures check(s) failed\n");
    exit(1);
}

echo "All tests passed\n";

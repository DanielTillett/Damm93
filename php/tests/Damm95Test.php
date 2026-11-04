<?php

/*
 Tests for the PHP base95 Damm check digit library. Dependency-free: run with
 `php php/tests/Damm95Test.php`.

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

declare(strict_types=1);

require_once __DIR__ . '/../src/Damm95.php';

use Damm95\Damm95;

$failures = 0;

function check(bool $condition, string $message): void
{
    global $failures;
    if (!$condition) {
        fwrite(STDERR, "FAIL: $message\n");
        $failures++;
    }
}

$alphabet = str_split(Damm95::ALPHABET);

/* Alphabet is a bijection. */
check(count(array_unique($alphabet)) === Damm95::BASE, 'alphabet has 95 distinct characters');
foreach ($alphabet as $i => $char) {
    check(Damm95::value($char) === $i, "value of '$char'");
}
for ($c = 0; $c < 256; $c++) {
    if (strpos(Damm95::ALPHABET, chr($c)) === false) {
        check(Damm95::value(chr($c)) === -1, "byte $c is not in the alphabet");
    }
}

/* Every single substitution and adjacent transposition is detected (exhaustive). */
foreach ($alphabet as $a) {
    $row = [];
    $column = [];
    foreach ($alphabet as $b) {
        $row[] = Damm95::checkDigit($a . $b);
        $column[] = Damm95::checkDigit($b . $a);
    }
    check(count(array_unique($row)) === Damm95::BASE, "row $a is a permutation");
    check(count(array_unique($column)) === Damm95::BASE, "column $a is a permutation");

    foreach ($alphabet as $x) {
        foreach ($alphabet as $y) {
            if ($x !== $y && Damm95::checkDigit($a . $x . $y) === Damm95::checkDigit($a . $y . $x)) {
                check(false, "transposition $a$x$y / $a$y$x not detected");
            }
        }
    }
}

/* Case-sensitivity and invalid input. */
check(Damm95::checkDigit('abc') !== Damm95::checkDigit('ABC'), 'input is case-sensitive');
check(Damm95::checkDigit("ab\tc") === Damm95::INVALID, 'tab is invalid');
check(Damm95::checkDigit('Hello world') === '\\', 'Hello world -> \\');
check(Damm95::checkDigit('café') === Damm95::INVALID, 'non-ASCII is invalid');
check(Damm95::checkDigit('Hello') === '8', 'Hello -> 8');
check(Damm95::isValid('Hello8'), 'Hello8 is valid');
check(!Damm95::isValid(''), 'empty string is invalid');

/* Shared vectors, identical to those used by the C and JavaScript tests. */
$count = 0;
foreach (file(__DIR__ . '/../../tests/vectors.txt', FILE_IGNORE_NEW_LINES) as $line) {
    if (strpos($line, "\t") === false) {
        continue;
    }
    [$code, $expected] = explode("\t", $line, 2);
    $expected = $expected === '' ? Damm95::INVALID : $expected;
    check(Damm95::checkDigit($code) === $expected, "vector $code");
    $count++;
}
check($count > 1000, 'shared vectors were read');

if ($failures > 0) {
    fwrite(STDERR, "$failures check(s) failed\n");
    exit(1);
}

echo "All tests passed\n";

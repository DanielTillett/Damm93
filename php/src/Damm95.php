<?php

/*
 Base95 (printable ASCII) implementation of the Damm error detection
 algorithm in PHP

 Copyright (c) 2026, Daniel Tillett
 SPDX-License-Identifier: BSD-2-Clause
 */

declare(strict_types=1);

namespace Damm95;

final class Damm95
{
    public const BASE = 95;

    /*
     Every printable ASCII character, space to '~', ordered so that base95
     zero is '0' and the digits keep their values. Input is case-sensitive.
     */
    public const ALPHABET = '0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'
        . ' !"#$%&\'()*+,-./';

    /* Returned by checkDigit() when the input contains a character outside the alphabet. */
    public const INVALID = null;

    /*
     The quasigroup operation x * y = 2(x - y) mod 95. It has a zero diagonal
     and is totally anti-symmetric, so every single-character error and
     adjacent transposition is detected.
     */
    private static function op(int $x, int $y): int
    {
        return 2 * ($x - $y + self::BASE) % self::BASE;
    }

    /**
     * Returns the value (0-94) of a base95 character, or -1 if it is not printable ASCII.
     */
    public static function value(string $char): int
    {
        if (strlen($char) !== 1) {
            return -1;
        }

        $byte = ord($char);

        return ($byte < 32 || $byte > 126) ? -1 : ($byte - 48 + self::BASE) % self::BASE;
    }

    /**
     * Returns the base95 check digit for a string, or INVALID (null) if the
     * string contains a character outside the alphabet.
     */
    public static function checkDigit(string $code): ?string
    {
        $interim = 0;

        for ($i = 0, $len = strlen($code); $i < $len; $i++) {
            $value = self::value($code[$i]);

            if ($value < 0) {
                return self::INVALID;
            }

            $interim = self::op($interim, $value);
        }

        return self::ALPHABET[$interim];
    }

    /**
     * Returns true if the last character of a string is a valid Damm check
     * digit for the characters before it.
     */
    public static function isValid(string $code): bool
    {
        return $code !== '' && self::checkDigit($code) === self::ALPHABET[0];
    }
}

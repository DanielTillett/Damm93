/** Every visible ASCII character except backslash, ordered '0' to '~' then '!' to '/' (input is case-sensitive). */
export const ALPHABET: string;

/** Returned by checkDigit() when the input contains a character outside the alphabet. */
export const INVALID: null;

/** Returns the value (0-92) of a base93 character, or -1 if it is not in the alphabet. */
export function value(char: string): number;

/** Returns the base93 check digit for a string, or INVALID (null) for characters outside the alphabet. */
export function checkDigit(code: string): string | null;

/** Returns true if the last character is a valid Damm check digit for the characters before it. */
export function isValid(code: string): boolean;

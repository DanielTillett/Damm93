# damm93: visible ASCII Damm check digits

The [Damm algorithm](https://en.wikipedia.org/wiki/Damm_algorithm) is a check digit algorithm that detects every single-character error and every adjacent transposition. Prepending leading zeros does not change the check digit.

**damm93** applies it to the 93 visible ASCII characters other than backslash, with matching implementations in C, PHP and JavaScript. Each one is a library plus a command-line tool, and all three give identical output.

The alphabet runs from `0` to `~` (skipping backslash), then `!` to `/`, so base93 zero is `0` and the digits keep their face values:

```
0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~!"#$%&'()*+,-./
```

Input is **case-sensitive**. Space, backslash, control characters (tab, newline, `DEL`) and anything outside ASCII are invalid.

Things to know before using damm93:

- **The check digit is always visible.** Space is not in the alphabet, so trimming whitespace can never remove a check digit, and codes cannot contain spaces.
- **No backslash either.** Backslash is the escape character in JSON, C, regular expressions and most shells, so leaving it out removes the worst quoting problems. The check digit can still be a quote, `$`, `&` or similar, so quote codes on the command line (`damm93 -a 'x=1;y=2'`) and escape them in SQL and shell scripts.
- **Damm only guarantees single-character errors and adjacent swaps.** That's ideal for human typing errors. For data corrupted in transit, a CRC detects far more kinds of damage.

## Usage

```bash
npm install damm93
```

```js
import { checkDigit, isValid, value, INVALID } from 'damm93';

checkDigit('World');  // 'q', or INVALID (null)
isValid('Worldq');    // true
value('H');           // 0-92, or -1 if not in the alphabet
```

The package is a dependency-free ES module with TypeScript declarations, and includes the `damm93` command (`npx damm93`). C and PHP implementations with identical output are in the [GitHub repository](https://github.com/DanielTillett/Damm93).

## Licence

BSD 2-Clause

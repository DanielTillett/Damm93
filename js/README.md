# damm95: printable ASCII Damm check digits

The [Damm algorithm](https://en.wikipedia.org/wiki/Damm_algorithm) is a check digit algorithm that detects every single-character error and every adjacent transposition. Prepending leading zeros does not change the check digit.

**damm95** applies it to all 95 printable ASCII characters, from space to `~`, with matching implementations in C, PHP and JavaScript. Each one is a library plus a command-line tool, and all three give identical output.

The alphabet runs from `0` to `~`, then space to `/`, so base95 zero is `0` and the digits keep their face values:

```
0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~ !"#$%&'()*+,-./
```

Input is **case-sensitive**. Control characters (tab, newline, `DEL`) and anything outside ASCII are invalid.

Things to know before using damm95:

- **The check digit can be any printable character**, including a space, a quote, a backslash or `$`. Quote codes on the command line (`damm95 -a 'Hello world'`) and escape them in JSON, SQL and shell scripts.
- **A check digit of space is easy to lose.** Anything that trims whitespace will silently remove it, and the code then fails validation.
- **Damm only guarantees single-character errors and adjacent swaps.** That's ideal for human typing errors. For data corrupted in transit, a CRC detects far more kinds of damage.

## Usage

```bash
npm install damm95
```

```js
import { checkDigit, isValid, value, INVALID } from 'damm95';

checkDigit('Hello');  // '8', or INVALID (null)
isValid('Hello8');    // true
value('H');           // 0-94, or -1 if not printable ASCII
```

The package is a dependency-free ES module with TypeScript declarations, and includes the `damm95` command (`npx damm95`). C and PHP implementations with identical output are in the [GitHub repository](https://github.com/DanielTillett/Damm95).

## Licence

BSD 2-Clause

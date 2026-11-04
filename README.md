# damm93: visible ASCII Damm check digits

The [Damm algorithm](https://en.wikipedia.org/wiki/Damm_algorithm) is a check digit algorithm that detects every single-character error and every adjacent transposition. Prepending leading zeros does not change the check digit.

**damm93** applies it to the 93 visible ASCII characters other than backslash, with matching implementations in C, PHP and JavaScript. Each one is a library plus a command-line tool, and all three give identical output.

The alphabet runs from `0` to `~` (skipping backslash), then `!` to `/`, so base93 zero is `0` and the digits keep their face values:

```
0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~!"#$%&'()*+,-./
```

Input is **case-sensitive**. Space, backslash, control characters (tab, newline, `DEL`) and anything outside ASCII are invalid.

| Implementation | Location | Requires |
| --- | --- | --- |
| C | [`include/`](include), [`src/`](src) | C99 compiler, CMake 3.16+ |
| PHP | [`php/`](php) | PHP 7.4+ |
| JavaScript | [`js/`](js) | Node.js 20+, or any modern browser |

## Which version should I use?

| | [damm32](https://github.com/DanielTillett/Damm32) | [damm64](https://github.com/DanielTillett/Damm64) | damm93 |
| --- | --- | --- | --- |
| Alphabet | Lowercase letters and digits without look-alikes, plus `*` | base64url | Visible ASCII except `\` |
| Bits per character | 5 | 6 | ~6.5 |
| Case | Insensitive | Sensitive | Sensitive |
| Best for | Codes people read and type | IDs and tokens that are copied, or used in URLs | Compact machine-to-machine strings |

Things to know before using damm93:

- **The check digit is always visible.** Space is not in the alphabet, so trimming whitespace can never remove a check digit, and codes cannot contain spaces.
- **No backslash either.** Backslash is the escape character in JSON, C, regular expressions and most shells, so leaving it out removes the worst quoting problems. The check digit can still be a quote, `$`, `&` or similar, so quote codes on the command line (`damm93 -a 'x=1;y=2'`) and escape them in SQL and shell scripts.
- **Damm only guarantees single-character errors and adjacent swaps.** That's ideal for human typing errors. For data corrupted in transit, a CRC detects far more kinds of damage.

## How it works

The 93 × 93 quasigroup is not stored as a table. It is computed from one formula:

> x ∗ y = 2 · (x − y) mod 93

Subtraction puts zeros on the diagonal, so a correct code ends in `0`. Because 93 is odd, 2 and 2² − 2 are both invertible modulo 93, which makes the operation totally anti-symmetric, so every adjacent transposition is detected. The same formula works for any odd base, which is why the alphabet drops two characters rather than one: the 94 visible characters would make an even base, where it fails. The tests check every property exhaustively, all 93³ combinations, in all three languages.

## C

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

```bash
cmake --build build
```

```bash
ctest --test-dir build --output-on-failure
```

```bash
cmake --install build
```

Without CMake:

```bash
cc -std=c99 -O2 -Iinclude src/damm93.c src/main.c -o damm93
```

```c
#include "damm93.h"

char digit = damm93_check_digit("World");  /* 'q', or DAMM93_INVALID ('\0') */
bool ok    = damm93_is_valid("Worldq");    /* true */
int value  = damm93_value('H');            /* 0-92, or -1 if not in the alphabet */
```

## Command-line usage

All three implementations share the same interface:

```
damm93 [-a | -c] [code ...]

  -a, --append   print each code with its check digit appended
  -c, --check    print nothing; exit 0 only if every code is valid
  -h, --help     show this help
  -V, --version  show the version
```

With no codes on the command line, codes are read from standard input, **one per line**. Put `--` before a code that starts with `-`. Exit status is 0 on success, 1 if a code is invalid or contains a character outside the alphabet, and 2 on a usage error.

```
$ damm93 World
Base93 Number: World
Base93 Check Digit: q
Error Check: 0

$ damm93 -a 'x=1;y=2'
x=1;y=2L

$ damm93 -c Worldq && echo valid
valid
```

## PHP

```bash
composer require danieltillett/damm93
```

```php
use Damm93\Damm93;

Damm93::checkDigit('World');  // 'q', or Damm93::INVALID (null)
Damm93::isValid('Worldq');    // true
Damm93::value('H');           // 0-92, or -1 if not in the alphabet
```

The command-line tool is installed as `vendor/bin/damm93`. Without Composer, `require` [`php/src/Damm93.php`](php/src/Damm93.php) directly. Run the tests with `php php/tests/Damm93Test.php`.

## JavaScript

```bash
npm install damm93
```

```js
import { checkDigit, isValid, value, INVALID } from 'damm93';

checkDigit('World');  // 'q', or INVALID (null)
isValid('Worldq');    // true
value('H');           // 0-92, or -1 if not in the alphabet
```

The package is a dependency-free ES module with TypeScript declarations, and includes the `damm93` command (`npx damm93`). Run the tests with `npm test --prefix js`.

## Testing

[`tests/vectors.txt`](tests/vectors.txt) holds every one- and two-character code plus random longer and invalid codes, with their check digits, generated by an independent Python reference implementation. Each line is `code<TAB>digit`; an empty digit means invalid, and lines without a tab are comments (codes can start with `#`, so `#` cannot mark a comment). The C, PHP and JavaScript suites all check against it, and CI runs them on Linux, macOS and Windows (C), PHP 7.4–8.4 and Node.js 20–24.

## Licence

BSD 2-Clause. See [LICENSE](LICENSE).

# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build commands

Build uses GNU Make with `gcc` (`-Wall -Wextra -O2 -std=c17`). The Makefile auto-detects the platform via `$(OS)` and selects the right exec name, ZIP commands, and shell utilities — Windows uses `cmd`/PowerShell wrappers, everything else uses Unix tools.

- `make` — compile to `./build/wordle` (or `wordle.exe` on Windows)
- `make zip` — compile, copy the executable into `./zip_content/`, then produce `./out/wordle_linux_x64.zip` or `./out/wordle_win64.zip`
- `make clean` — remove the `./build/` tree and the staged executable in `./zip_content/`

There is no test suite, lint config, or CI. Changes are validated by running the produced binary from a directory containing `wordlist.txt` (the binary `fopen`s it via a relative path — running from elsewhere fails immediately).

## Architecture

Tiny single-binary console game. ~5 source files, no third-party deps. The interesting parts are not the structure but the runtime contracts:

- **`game_t` lifecycle is split across three calls** in `main.c`: `game_init` allocates the wordlist and picks the secret word, `game_loop` runs the turn loop, `game_end` frees the wordlist *and* the `game_t` itself and restores the console. `game_init` calls `exit(EXIT_FAILURE)` directly on I/O or allocation errors — there is no error return path. If you add a new failure mode, follow the same pattern or you will leak the wordlist.
- **Wordlist parsing happens once at startup** (`game_init` in `src/game.c`). It reads `wordlist.txt` line-by-line with a growing buffer, skips blanks and `#` comments, trims whitespace, uppercases, and rejects anything that isn't exactly `WORD_LENGTH` ASCII letters. The wordlist array also doubles in size on demand. Both behaviors matter: the file format spec in the README ("comments start with #, words are 5 letters") is enforced here, not validated elsewhere.
- **Letter-coloring uses a two-pass count algorithm** in `game_print_guess` to handle duplicate letters correctly (e.g. guessing `LLAMA` against `HELLO`). Pass 1 marks greens and decrements a per-letter remaining-count; pass 2 marks yellows only while that count is still positive. Editing this loop without preserving the count semantics will reintroduce the duplicate-letter bug fixed in commit `e69ae5e` ("Rework letter counting").
- **Console rendering is ANSI-escape-only** (`\x1b[...]` sequences for color, cursor moves, line clears). On Windows, `setup_console`/`restore_console` in `src/console.c` toggle `ENABLE_VIRTUAL_TERMINAL_PROCESSING` on the stdout handle and save/restore the prior mode — call them via `main` so the terminal isn't left in a weird state on exit. On non-Windows these are no-ops apart from a final reset.
- **Constants live in `src/definitions.h`**: `WORD_LENGTH = 5`, `MAX_TURNS = 6`, `ALPHABET_SIZE = 26`. `ALPHABET_SIZE` is used to size the per-letter count array, so it must match the A–Z assumption baked into the validation/coloring code.

## Distribution layout

`./zip_content/` holds the bundled-game payload (`wordlist.txt`, `license.txt`, `readme.txt`). `make zip` copies the freshly built executable into this directory and zips the whole folder into `./out/`. The wordlist shipped to users is the one in `./zip_content/wordlist.txt` — edit it there, not anywhere else.

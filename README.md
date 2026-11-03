# Sudoku

A classic Sudoku game for the command line, written in plain C++. Generate a random puzzle at three difficulty levels, or type in your own and play it. Designed to be simple and dependency-free so it can run on very old systems, including **Windows 2000**.

```
====== SUDOKU ======

    1 2 3   4 5 6   7 8 9
  +-------+-------+-------+
1 | 5 3 . | . 7 . | . . . |
2 | 6 . . | 1 9 5 | . . . |
3 | . 9 8 | . . . | . 6 . |
  +-------+-------+-------+
4 | 8 . . | . 6 . | . . 3 |
5 | 4 . . | 8 . 3 | . . 1 |
6 | 7 . . | . 2 . | . . 6 |
  +-------+-------+-------+
7 | . 6 . | . . . | 2 8 . |
8 | . . . | 4 1 9 | . . 5 |
9 | . . . | . 8 . | . 7 9 |
  +-------+-------+-------+
```

## Features

- **Three difficulty levels**: Easy, Normal and Hard (30, 40 or 50 numbers removed from a full board).
- **Unique-solution puzzles**: every generated puzzle is checked so it has exactly one solution.
- **Enter your own puzzle**: type in a puzzle from a newspaper or app. The game validates it (rule violations, no solution, or multiple solutions) and works out the answer for you.
- **Hints**: reveal the next missing number (the game counts how many you use).
- **Mistake checker**: see which of your numbers are wrong.
- **Possible numbers**: ask which numbers can legally go in any empty cell.
- **Show solution**: give up and see the full answer.
- **Credits screen** and a play-again loop.

## How to play

When the game starts, choose an option from the main menu:

| Option | Action |
|--------|--------|
| 1 | Easy puzzle |
| 2 | Normal puzzle |
| 3 | Hard puzzle |
| 4 | Type in your own puzzle |
| 5 | Credits |

During a game:

| Option | Action |
|--------|--------|
| 1 | Put a number (asks for row, column, then number) |
| 2 | Erase a number |
| 3 | Hint |
| 4 | Check my mistakes |
| 5 | Show possible numbers for a cell |
| 6 | Show the solution (ends the game) |
| 0 | Quit to the menu |

Rows and columns are numbered 1 to 9. The starting numbers of a puzzle are fixed and can't be changed.

### Entering your own puzzle

Type the puzzle one row at a time as 9 characters with no spaces. Use `0` or `.` for empty cells. For example:

```
Row 1: 530070000
Row 2: 600195000
Row 3: 098000060
...
```

## Building

The program only uses standard C++ and the `cls`/`clear` command, so it has no external dependencies.

**Any modern compiler (Windows, Linux, macOS):**

```
g++ -O2 -o sudoku sudoku.cpp
```

**Visual C++ 6.0 / Visual Studio .NET 2003:**

```
cl /EHsc /O2 /MT sudoku.cpp
```

**Old MinGW (GCC 3.x/4.x):**

```
g++ -O2 -static -o sudoku.exe sudoku.cpp
```

You can also build it in an IDE such as Code::Blocks by creating a console project and adding `sudoku.cpp`.

### Windows 2000 notes

- The source contains no Windows API calls, so the code itself is compatible.
- Whether the compiled `.exe` runs on Windows 2000 depends on the **toolchain** you build with. Visual Studio 2010 and newer produce programs that need Windows XP SP2 or later, regardless of the code. Use an older compiler from that era.
- Build with a **static runtime** (`/MT` for Visual C++, `-static` for MinGW) so you don't need to install extra DLLs on the old machine.
- The file includes a small workaround so Visual C++ 6.0's old `for`-loop scoping rules don't break the code.

## How it works

- **Generation**: a full valid board is filled in using randomized backtracking.
- **Removing numbers**: numbers are removed one at a time at random. After each removal, a solution counter (which always picks the empty cell with the fewest options) confirms the puzzle still has exactly one solution. If not, the number is put back.
- **Solving**: user-entered puzzles are solved with plain backtracking to produce the answer used for hints and checking.

## Project structure

```
sudoku.cpp    # the whole game in a single source file
README.md
```

## Credits

Made by **Jabez Martinez**.

## License

Add a license of your choice here (for example MIT) by including a `LICENSE` file in the repository.

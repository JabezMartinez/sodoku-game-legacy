// Console Sudoku - Windows 2000 compatible build
//
// Build with an OLD toolchain (see notes at the bottom of this file):
//   Visual C++ 6.0 / VS .NET 2003:  cl /EHsc /O2 /MT sudoku.cpp
//   Old MinGW (GCC 3.x/4.x):        g++ -O2 -static -o sudoku.exe sudoku.cpp

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

// Visual C++ 6.0 uses the old "for" scoping rule, where a variable declared
// in a for loop stays alive after the loop. That breaks code that reuses
// "int i" in several loops in the same function. This makes VC6 behave
// like modern compilers. (Must come AFTER the #includes.)
#if defined(_MSC_VER) && (_MSC_VER < 1300)
    #define for if (0) {} else for
#endif

int board[9][9];       // the board the player sees
int solution[9][9];    // the full answer
int fixedCell[9][9];   // 1 if the number was there from the start
int solCount = 0;      // used when counting solutions
int hintsUsed = 0;

// clears the console (cls on Windows, clear everywhere else)
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// reads a number from the user (keeps asking if they type letters)
int readNumber() {
    int x;
    while (!(cin >> x)) {
        if (cin.eof()) exit(0);
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Please type a number: ";
    }
    return x;
}

void waitForEnter() {
    cout << "Press enter to continue...";
    cin.ignore(1000, '\n');
    cin.get();
}

// check if num can go in this spot
bool canPlace(int g[9][9], int row, int col, int num) {
    // check row and column
    for (int i = 0; i < 9; i++) {
        if (g[row][i] == num) return false;
        if (g[i][col] == num) return false;
    }
    // check the 3x3 box
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (g[startRow + i][startCol + j] == num) return false;
        }
    }
    return true;
}

// fills the grid with a random complete sudoku
bool fillGrid(int g[9][9]) {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (g[row][col] == 0) {
                // make a shuffled list of 1-9
                int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
                for (int i = 8; i > 0; i--) {
                    int j = rand() % (i + 1);
                    int temp = nums[i];
                    nums[i] = nums[j];
                    nums[j] = temp;
                }
                for (int k = 0; k < 9; k++) {
                    if (canPlace(g, row, col, nums[k])) {
                        g[row][col] = nums[k];
                        if (fillGrid(g)) return true;
                        g[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;  // no empty cells left
}

// solves a puzzle (used for the puzzle the player types in)
bool solveGrid(int g[9][9]) {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (g[row][col] == 0) {
                for (int n = 1; n <= 9; n++) {
                    if (canPlace(g, row, col, n)) {
                        g[row][col] = n;
                        if (solveGrid(g)) return true;
                        g[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

// counts how many solutions there are (stops at 2)
void countSolutions(int g[9][9]) {
    if (solCount >= 2) return;

    // find the empty cell with the fewest options
    int bestRow = -1, bestCol = -1, bestOptions = 10;
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (g[row][col] == 0) {
                int options = 0;
                for (int n = 1; n <= 9; n++) {
                    if (canPlace(g, row, col, n)) options++;
                }
                if (options < bestOptions) {
                    bestOptions = options;
                    bestRow = row;
                    bestCol = col;
                }
            }
        }
    }

    if (bestRow == -1) {   // board is full so it is a solution
        solCount++;
        return;
    }

    for (int n = 1; n <= 9; n++) {
        if (canPlace(g, bestRow, bestCol, n)) {
            g[bestRow][bestCol] = n;
            countSolutions(g);
            g[bestRow][bestCol] = 0;
        }
    }
}

// remember which cells are fixed (the starting numbers)
void setFixedCells() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] != 0) fixedCell[i][j] = 1;
            else fixedCell[i][j] = 0;
        }
    }
    hintsUsed = 0;
}

// makes a new puzzle, removeCount is how many numbers to take out
void makePuzzle(int removeCount) {
    // clear everything first
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            board[i][j] = 0;
        }
    }
    fillGrid(board);

    // save the answer
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            solution[i][j] = board[i][j];
        }
    }

    // take out numbers one by one
    int removed = 0;
    int tries = 0;
    while (removed < removeCount && tries < 500) {
        tries++;
        int r = rand() % 9;
        int c = rand() % 9;
        if (board[r][c] == 0) continue;

        int backup = board[r][c];
        board[r][c] = 0;

        // make sure there is still only one solution
        int copy[9][9];
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                copy[i][j] = board[i][j];
            }
        }
        solCount = 0;
        countSolutions(copy);

        if (solCount == 1) {
            removed++;
        } else {
            board[r][c] = backup;  // put it back
        }
    }

    setFixedCells();
}

// lets the player type in their own puzzle, returns true if it worked
bool enterOwnPuzzle() {
    clearScreen();
    cout << "====== ENTER YOUR OWN PUZZLE ======" << endl << endl;
    cout << "Type the puzzle one row at a time." << endl;
    cout << "Use 9 digits with no spaces. Use 0 (or .) for empty cells." << endl;
    cout << "Example row: 530070000" << endl << endl;

    for (int i = 0; i < 9; i++) {
        string line;
        bool good = false;
        while (!good) {
            cout << "Row " << i + 1 << ": ";
            cin >> line;
            good = true;
            if (line.length() != 9) {
                cout << "A row needs exactly 9 digits, try again." << endl;
                good = false;
            } else {
                for (int j = 0; j < 9; j++) {
                    char ch = line[j];
                    if (ch == '.') ch = '0';
                    if (ch < '0' || ch > '9') {
                        cout << "Only digits are allowed, try again." << endl;
                        good = false;
                        break;
                    }
                    board[i][j] = ch - '0';
                }
            }
        }
    }

    // check that the numbers don't break the rules
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] != 0) {
                int num = board[i][j];
                board[i][j] = 0;
                bool ok = canPlace(board, i, j, num);
                board[i][j] = num;
                if (!ok) {
                    cout << endl << "Row " << i + 1 << " col " << j + 1
                         << " breaks the sudoku rules." << endl;
                    return false;
                }
            }
        }
    }

    // check the number of solutions
    int copy[9][9];
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            copy[i][j] = board[i][j];
        }
    }
    solCount = 0;
    countSolutions(copy);
    if (solCount == 0) {
        cout << endl << "This puzzle has no solution." << endl;
        return false;
    }
    if (solCount > 1) {
        cout << endl << "This puzzle has more than one solution. Add more numbers." << endl;
        return false;
    }

    // find the answer and save it
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            solution[i][j] = board[i][j];
        }
    }
    solveGrid(solution);
    setFixedCells();
    return true;
}

// shows the credits screen
void showCredits() {
    clearScreen();
    cout << "==============================" << endl;
    cout << "          CREDITS" << endl;
    cout << "==============================" << endl;
    cout << "    Made by: Jabez Martinez" << endl;
    cout << "==============================" << endl;
    cout << endl;
}

void printBoard() {
    clearScreen();
    cout << "====== SUDOKU ======" << endl << endl;
    cout << "    1 2 3   4 5 6   7 8 9" << endl;
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0) {
            cout << "  +-------+-------+-------+" << endl;
        }
        cout << i + 1 << " | ";
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == 0) cout << ". ";
            else cout << board[i][j] << " ";
            if (j % 3 == 2) cout << "| ";
        }
        cout << endl;
    }
    cout << "  +-------+-------+-------+" << endl << endl;
}

bool isSolved() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] != solution[i][j]) return false;
        }
    }
    return true;
}

// shows which numbers are wrong
void checkBoard() {
    int wrong = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] != 0 && board[i][j] != solution[i][j]) {
                wrong++;
                cout << "Wrong number at row " << i + 1 << " col " << j + 1 << endl;
            }
        }
    }
    if (wrong == 0) cout << "No mistakes so far, keep going!" << endl;
}

// fills in one empty cell with the right number
void giveHint() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] != solution[i][j]) {
                board[i][j] = solution[i][j];
                fixedCell[i][j] = 1;
                hintsUsed++;
                cout << "Hint: row " << i + 1 << " col " << j + 1 << " is " << solution[i][j] << endl;
                return;
            }
        }
    }
}

// shows which numbers can go in a cell
void showPossible() {
    cout << "Row (1-9): ";
    int row = readNumber();
    cout << "Column (1-9): ";
    int col = readNumber();
    if (row < 1 || row > 9 || col < 1 || col > 9) {
        cout << "Row and column must be between 1 and 9." << endl;
        return;
    }
    if (board[row - 1][col - 1] != 0) {
        cout << "That cell already has a number." << endl;
        return;
    }
    cout << "You can try: ";
    for (int n = 1; n <= 9; n++) {
        if (canPlace(board, row - 1, col - 1, n)) cout << n << " ";
    }
    cout << endl;
}

// asks for a row, column and number and puts it on the board
void putNumber(bool erase) {
    cout << "Row (1-9): ";
    int row = readNumber();
    cout << "Column (1-9): ";
    int col = readNumber();
    if (row < 1 || row > 9 || col < 1 || col > 9) {
        cout << "Row and column must be between 1 and 9." << endl;
        waitForEnter();
        return;
    }
    if (fixedCell[row - 1][col - 1] == 1) {
        cout << "You can't change that cell!" << endl;
        waitForEnter();
        return;
    }

    if (erase) {
        board[row - 1][col - 1] = 0;
        return;
    }

    cout << "Number (1-9): ";
    int num = readNumber();
    if (num < 1 || num > 9) {
        cout << "The number must be between 1 and 9." << endl;
        waitForEnter();
        return;
    }
    board[row - 1][col - 1] = num;
    if (num != solution[row - 1][col - 1]) {
        cout << "Hmm, that number doesn't look right..." << endl;
        waitForEnter();
    }
}

void playGame() {
    while (true) {
        printBoard();
        cout << "What do you want to do?" << endl;
        cout << "1. Put a number" << endl;
        cout << "2. Erase a number" << endl;
        cout << "3. Hint" << endl;
        cout << "4. Check my mistakes" << endl;
        cout << "5. Show possible numbers for a cell" << endl;
        cout << "6. Show the solution" << endl;
        cout << "0. Quit" << endl;
        cout << "> ";
        int choice = readNumber();

        if (choice == 0) {
            return;
        } else if (choice == 1) {
            putNumber(false);
        } else if (choice == 2) {
            putNumber(true);
        } else if (choice == 3) {
            giveHint();
            waitForEnter();
        } else if (choice == 4) {
            checkBoard();
            waitForEnter();
        } else if (choice == 5) {
            showPossible();
            waitForEnter();
        } else if (choice == 6) {
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    board[i][j] = solution[i][j];
                }
            }
            printBoard();
            cout << "Here is the solution." << endl;
            return;
        } else {
            cout << "Please choose a number from the menu." << endl;
            waitForEnter();
        }

        if (isSolved()) {
            printBoard();
            cout << "YOU WIN!!! You used " << hintsUsed << " hints." << endl;
            return;
        }
    }
}

int main() {
    srand((unsigned int)time(0));
    string again = "y";

    while (again == "y" || again == "Y") {
        clearScreen();
        cout << "====== SUDOKU ======" << endl;
        cout << "Choose an option:" << endl;
        cout << "1. Easy" << endl;
        cout << "2. Normal" << endl;
        cout << "3. Hard" << endl;
        cout << "4. Type in my own puzzle" << endl;
        cout << "5. Credits" << endl;
        cout << "> ";
        int choice = readNumber();

        if (choice >= 1 && choice <= 3) {
            // how many numbers to remove from the full board
            int removeCount;
            if (choice == 1) removeCount = 30;
            else if (choice == 2) removeCount = 40;
            else removeCount = 50;

            cout << "Making puzzle, please wait..." << endl;
            makePuzzle(removeCount);
        } else if (choice == 4) {
            if (!enterOwnPuzzle()) {
                waitForEnter();
                continue;
            }
        } else if (choice == 5) {
            showCredits();
            waitForEnter();
            continue;
        } else {
            cout << "Please choose 1, 2, 3, 4 or 5." << endl;
            waitForEnter();
            continue;
        }

        playGame();

        cout << "Play again? (y/n): ";
        cin >> again;
    }

    cout << "Thanks for playing!" << endl;
    return 0;
}

// ---------------------------------------------------------------------
// NOTES FOR WINDOWS 2000
// - Use a compiler from that era. Visual Studio 2010 and newer produce
//   programs that need Windows XP SP2 or later, no matter what the code says.
// - Build with /MT (static runtime) so you don't have to install any
//   extra DLLs on the old machine.
// - The program only uses standard C++ and the "cls" command, which
//   Windows 2000's cmd.exe has, so no Windows API calls are needed.
// ---------------------------------------------------------------------

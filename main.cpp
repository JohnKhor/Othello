/**
	@author Khor Kia Kin
*/

#include <iostream>
#include <vector>
#include <ctime> // time
#include <cstdlib> // srand, rand
using namespace std;

void pauseScreen(string& input) {
    cout << "Press ENTER to continue." << endl;
    getline(cin, input);
}

void resetBoard(char board[8][8]) {
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            board[row][col] = ' ';
    board[3][3] = board[4][4] = 'O';
    board[3][4] = board[4][3] = 'X';
}

int score(char board[8][8], char disc) {
    int score = 0;
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            if (board[row][col] == disc)
                score++;
    return score;
}

void printBoard(char board[8][8]) {
    for (int row = 0; row < 8; row++) {
        cout << "  +---+---+---+---+---+---+---+---+" << endl;
        cout << row + 1;
        for (int col = 0; col < 8; col++) cout << " | " << board[row][col];
        cout << " | " << endl;
    }
    cout << "  +---+---+---+---+---+---+---+---+" << endl;
    cout << "    a   b   c   d   e   f   g   h" << endl;
    cout << "  Score: O = " << score(board, 'O') << " X = " << score(board, 'X') << endl;
}

void initTurn(char& turn) {
    srand(time(0));
    turn = (rand() % 2 == 0) ? 'O' : 'X';
}

void switchTurn(char& turn) {
    turn = (turn == 'O') ? 'X' : 'O';
}

bool inBounds(int row, int col) {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}

bool placeDisc(char board[8][8], char disc, int row, int col, bool flip = true) {
    // Cell is out of bounds or occupied - not a legal move
    if (!inBounds(row, col) || board[row][col] != ' ') return false;

    vector<int> flipX, flipY;
    char otherDisc = (disc == 'O') ? 'X' : 'O';
    char stepX[8] = {-1, -1, -1, 0, 0, 1, 1, 1}, stepY[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int x, y;

    // Check eight directions
    for (int i = 0; i < 8; i++) {

        // First step in the direction
        x = row + stepX[i], y = col + stepY[i];

        // An opponent disc is nearby
        if (inBounds(x, y) && board[x][y] == otherDisc)

            // Second step in the direction
            x += stepX[i], y += stepY[i];

            while(inBounds(x, y)) {

                // An empty cell is in the direction - not a legal move
                if (board[x][y] == ' ')
                    break;

                // Our disc is in the direction - a legal move
                else if (board[x][y] == disc) {

                    // One step in the reverse direction
                    // because we don't have to flip our disc again
                    x -= stepX[i], y -= stepY[i];

                    // Note down the discs to flip while going in the reverse direction
                    while(!(x == row && y == col)) {
                        flipX.push_back(x);
                        flipY.push_back(y);
                        x -= stepX[i], y -= stepY[i];
                    }

                    break;
                }

                // Another step in the direction
                x += stepX[i], y += stepY[i];
            }
    }

    // No disc to flip - not a legal move
    if (flipX.empty()) return false;

    // Got disc to flip - a legal move
    if (flip) { // if the fifth parameter is true - we actually place our disc and flip the discs
        board[row][col] = disc;
        for (unsigned int i = 0; i < flipX.size(); i++)
            board[flipX[i]][flipY[i]] = disc;
    }
    return true;
}

void addHint(char board[8][8], char disc) {
    vector<int> x, y;
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            if (placeDisc(board, disc, row, col, false)) {
                // board[row][col] = '#'
                // not used because the '#' will interfere with placeDisc function
                x.push_back(row);
                y.push_back(col);
            }
    for (int i = 0; i < x.size(); i++)
        board[x[i]][y[i]] = '#';
}

void eraseHint(char board[8][8]) {
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            if (board[row][col] == '#')
                board[row][col] = ' ';
}

bool hasMove(char board[8][8], char disc) {
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            if (placeDisc(board, disc, row, col, false))
                return true;
    return false;
}

bool ai(char board[8][8]) {
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            if (placeDisc(board, 'X', row, col))
                return true;
    return false;
}

bool isOver(char board[8][8], int& win) {
    if (hasMove(board, 'O') || hasMove(board, 'X')) return false;

    int scoreO = score(board, 'O'), scoreX = score(board, 'X');
    if (scoreO == scoreX) win = -1;
    else if (scoreO < scoreX) win = 0;
    else win = 1;
    return true;
}

int main() {
    char board[8][8], turn;
    int row, col, win;
    string input;

    // The main game loop
    while (true) {

        // Initialise game
        resetBoard(board);
        initTurn(turn);

        // Game is running
        while (!isOver(board, win)) {

            // Render game
            addHint(board, turn);
            printBoard(board);
            eraseHint(board); // so they don't interfere with placeDisc function
            cout << "Current Player: " << turn << endl;

            // Human's turn
            if (turn == 'O') {
                cout << "Human's turn." << endl;
                if (!hasMove(board, 'O')) {
                    cout << "Sorry, you don't have a move available." << endl;
                    pauseScreen(input);
                } else {
                    while(true) {
                        cout << "-> ";
                        getline(cin, input);
                        // Correct input format
                        if (input.length() == 3 && input[1] == ' ') {
                            // Convert to int from ASCII
                            row = input[0] - '0' - 1; // minus 1 because array is zero-based
                            col = input[2] - 'a';
                            if (placeDisc(board, 'O', row, col))
                                break;
                            else
                                cout << "That move does not flip any discs. / Coordinate is out of bounds." << endl;
                        }
                        else if (input == "quit") break;
                        else cout << "Invalid input. Please type in the format of <row> <column>." << endl;
                    }
                }
            }
            // Computer's turn
            else if (turn == 'X') {
                cout << "Computer's turn." << endl;
                if (!ai(board)) cout << "Computer could not find a move." << endl;
                pauseScreen(input);
            }
            if (input == "quit") break;

            // Switch turn
            switchTurn(turn);
        }
        if (input == "quit") break;

        // Game finish
        printBoard(board);
        if (win == -1) cout << "It's a tie!" << endl;
        else if (win == 0) cout << "You have lost to the computer!" << endl;
        else if (win == 1) cout << "You have won the computer!" << endl;

        pauseScreen(input);
        if (input == "quit") break;
    }

    return 0;
}

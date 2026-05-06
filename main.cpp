#include <iostream>
#include <string>
#include <windows.h>
#include <cstdlib>
using namespace std;

class Board;

int myAbs(int x) {
    if (x < 0) return -x;
    return x;
}

char otherColor(char c) {
    if (c == 'W') return 'B';
    return 'W';
}

// Brings the console color back to normal after printing colored boxes
void setNormalColor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

// Gives the chessboard alternate tile colors
void setTileColor(int r, int c) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    // Light tile
    if ((r + c) % 2 == 0) {
        SetConsoleTextAttribute(h, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
    }
    // Dark tile
    else {
        SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }
}

// Clears the screen so the board refreshes after every command
void clearScreen() {
    setNormalColor();
    system("cls");
    setNormalColor();
}

// This pause lets the user read messages before the screen refreshes again
void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(1000, '\n');
    cin.get();
}

// Prints the main heading of the game
void printTitle() {
    cout << "OOP Project\n";
    cout << "Two-player console Chess\n";
    cout << "Type help to see commands.\n";
}

// This is the parent class for all chess pieces
// Common things like color and moved status are stored here
class Piece {
private:
    char color;
    bool moved;

public:
    Piece(char c) {
        color = c;
        moved = false;
    }

    virtual ~Piece() {}

    char getColor() const {
        return color;
    }

    bool hasMoved() const {
        return moved;
    }

    void setMoved(bool value) {
        moved = value;
    }

    // Returns the correct symbol for each chess piece
    // UTF-8 codes are used
    string symbol() const {
        char t = type();

        if (color == 'W') {
            if (t == 'K') return "\xE2\x99\x94";
            if (t == 'Q') return "\xE2\x99\x95";
            if (t == 'R') return "\xE2\x99\x96";
            if (t == 'B') return "\xE2\x99\x97";
            if (t == 'N') return "\xE2\x99\x98";
            return "\xE2\x99\x99";
        }
        else {
            if (t == 'K') return "\xE2\x99\x9A";
            if (t == 'Q') return "\xE2\x99\x9B";
            if (t == 'R') return "\xE2\x99\x9C";
            if (t == 'B') return "\xE2\x99\x9D";
            if (t == 'N') return "\xE2\x99\x9E";
            return "\xE2\x99\x9F";
        }
    }

    // Each child class tells which piece it is
    virtual char type() const = 0;

    // Each chess piece has its own movement rule
    virtual bool canMove(Board& board, int sr, int sc, int dr, int dc) const = 0;
};

class Pawn : public Piece {
public:
    Pawn(char c) : Piece(c) {}

    char type() const {
        return 'P';
    }

    bool canMove(Board& board, int sr, int sc, int dr, int dc) const;
};

class Rook : public Piece {
public:
    Rook(char c) : Piece(c) {}

    char type() const {
        return 'R';
    }

    bool canMove(Board& board, int sr, int sc, int dr, int dc) const;
};

class Knight : public Piece {
public:
    Knight(char c) : Piece(c) {}

    char type() const {
        return 'N';
    }

    bool canMove(Board& board, int sr, int sc, int dr, int dc) const;
};

class Bishop : public Piece {
public:
    Bishop(char c) : Piece(c) {}

    char type() const {
        return 'B';
    }

    bool canMove(Board& board, int sr, int sc, int dr, int dc) const;
};

class Queen : public Piece {
public:
    Queen(char c) : Piece(c) {}

    char type() const {
        return 'Q';
    }

    bool canMove(Board& board, int sr, int sc, int dr, int dc) const;
};

class King : public Piece {
public:
    King(char c) : Piece(c) {}

    char type() const {
        return 'K';
    }

    bool canMove(Board& board, int sr, int sc, int dr, int dc) const;
};

// Board stores the complete chessboard and most of the game rules
class Board {
private:
    Piece* cells[8][8];
    char turn;

    // These values help in checking en passant
    int epRow;
    int epCol;
    int epPawnRow;
    int epPawnCol;

public:
    Board() {
        turn = 'W';
        epRow = -1;
        epCol = -1;
        epPawnRow = -1;
        epPawnCol = -1;

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                cells[i][j] = 0;
            }
        }

        setup();
    }

    // Deletes all pieces that were created using new
    ~Board() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (cells[i][j] != 0) {
                    delete cells[i][j];
                }
            }
        }
    }

    // Places all pieces at the start of the game
    void setup() {
        cells[0][0] = new Rook('B');
        cells[0][1] = new Knight('B');
        cells[0][2] = new Bishop('B');
        cells[0][3] = new Queen('B');
        cells[0][4] = new King('B');
        cells[0][5] = new Bishop('B');
        cells[0][6] = new Knight('B');
        cells[0][7] = new Rook('B');

        for (int j = 0; j < 8; j++) {
            cells[1][j] = new Pawn('B');
        }

        for (int j = 0; j < 8; j++) {
            cells[6][j] = new Pawn('W');
        }

        cells[7][0] = new Rook('W');
        cells[7][1] = new Knight('W');
        cells[7][2] = new Bishop('W');
        cells[7][3] = new Queen('W');
        cells[7][4] = new King('W');
        cells[7][5] = new Bishop('W');
        cells[7][6] = new Knight('W');
        cells[7][7] = new Rook('W');
    }

    bool inside(int r, int c) const {
        return r >= 0 && r < 8 && c >= 0 && c < 8;
    }

    bool isEmpty(int r, int c) const {
        return inside(r, c) && cells[r][c] == 0;
    }

    bool hasEnemy(int r, int c, char color) const {
        return inside(r, c) && cells[r][c] != 0 && cells[r][c]->getColor() != color;
    }

    bool hasOwn(int r, int c, char color) const {
        return inside(r, c) && cells[r][c] != 0 && cells[r][c]->getColor() == color;
    }

    char getTurn() const {
        return turn;
    }

    string colorName(char c) const {
        if (c == 'W') return "White";
        return "Black";
    }

    // Checks if the path is clear for rook, bishop, and queen
    bool pathClear(int sr, int sc, int dr, int dc) const {
        int rStep = 0;
        int cStep = 0;

        if (dr > sr) rStep = 1;
        if (dr < sr) rStep = -1;
        if (dc > sc) cStep = 1;
        if (dc < sc) cStep = -1;

        int r = sr + rStep;
        int c = sc + cStep;

        while (r != dr || c != dc) {
            if (cells[r][c] != 0) return false;
            r = r + rStep;
            c = c + cStep;
        }

        return true;
    }

    // Converts input like e2 into array indexes
    bool posToIndex(string pos, int& r, int& c) const {
        if (pos.length() != 2) return false;

        char file = pos[0];
        char rank = pos[1];

        if (file >= 'A' && file <= 'H') {
            file = file + 32;
        }

        if (file < 'a' || file > 'h') return false;
        if (rank < '1' || rank > '8') return false;

        c = file - 'a';
        r = 8 - (rank - '0');

        return true;
    }

    // Converts row and column back into chess position
    string indexToPos(int r, int c) const {
        string s = "a1";
        s[0] = char('a' + c);
        s[1] = char('8' - r);
        return s;
    }

    // Checks if the given square can be used for en passant
    bool isEnPassantSquare(int r, int c, char color) const {
        if (r == epRow && c == epCol && epPawnRow != -1) {
            if (cells[epPawnRow][epPawnCol] != 0 && cells[epPawnRow][epPawnCol]->getColor() != color) {
                return true;
            }
        }

        return false;
    }

    // Checks all conditions needed for castling
    bool canCastle(char color, int sr, int sc, int dr, int dc) {
        if (!inside(sr, sc) || !inside(dr, dc)) return false;
        if (sr != dr) return false;
        if (sc != 4) return false;
        if (cells[sr][sc] == 0) return false;
        if (cells[sr][sc]->type() != 'K') return false;
        if (cells[sr][sc]->getColor() != color) return false;
        if (cells[sr][sc]->hasMoved()) return false;
        if (isInCheck(color)) return false;

        char enemy = otherColor(color);

        // King side castling
        if (dc == 6) {
            if (cells[sr][7] == 0 || cells[sr][7]->type() != 'R') return false;
            if (cells[sr][7]->getColor() != color || cells[sr][7]->hasMoved()) return false;
            if (cells[sr][5] != 0 || cells[sr][6] != 0) return false;
            if (isSquareAttacked(sr, 5, enemy)) return false;
            if (isSquareAttacked(sr, 6, enemy)) return false;
            return true;
        }

        // Queen side castling
        if (dc == 2) {
            if (cells[sr][0] == 0 || cells[sr][0]->type() != 'R') return false;
            if (cells[sr][0]->getColor() != color || cells[sr][0]->hasMoved()) return false;
            if (cells[sr][1] != 0 || cells[sr][2] != 0 || cells[sr][3] != 0) return false;
            if (isSquareAttacked(sr, 3, enemy)) return false;
            if (isSquareAttacked(sr, 2, enemy)) return false;
            return true;
        }

        return false;
    }

    // Checks whether a square is being attacked by the opponent
    bool isSquareAttacked(int r, int c, char byColor) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Piece* p = cells[i][j];

                if (p == 0 || p->getColor() != byColor) {
                    continue;
                }

                int rd = r - i;
                int cd = c - j;
                char t = p->type();

                if (t == 'P') {
                    int dir = 1;
                    if (byColor == 'W') dir = -1;

                    if (r == i + dir && (c == j + 1 || c == j - 1)) {
                        return true;
                    }
                }
                else if (t == 'N') {
                    if ((myAbs(rd) == 2 && myAbs(cd) == 1) || (myAbs(rd) == 1 && myAbs(cd) == 2)) {
                        return true;
                    }
                }
                else if (t == 'B') {
                    if (myAbs(rd) == myAbs(cd) && pathClear(i, j, r, c)) {
                        return true;
                    }
                }
                else if (t == 'R') {
                    if ((i == r || j == c) && pathClear(i, j, r, c)) {
                        return true;
                    }
                }
                else if (t == 'Q') {
                    if (((i == r || j == c) || myAbs(rd) == myAbs(cd)) && pathClear(i, j, r, c)) {
                        return true;
                    }
                }
                else if (t == 'K') {
                    if (myAbs(rd) <= 1 && myAbs(cd) <= 1) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    // Finds the king and checks if it is under attack
    bool isInCheck(char color) {
        int kr = -1;
        int kc = -1;

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (cells[i][j] != 0 && cells[i][j]->type() == 'K' && cells[i][j]->getColor() == color) {
                    kr = i;
                    kc = j;
                }
            }
        }

        if (kr == -1) return true;

        return isSquareAttacked(kr, kc, otherColor(color));
    }

    // Tries a move temporarily to see if our own king becomes unsafe
    bool wouldLeaveKingInCheck(int sr, int sc, int dr, int dc, char color) {
        Piece* moving = cells[sr][sc];
        Piece* dest = cells[dr][dc];

        bool castle = false;
        bool enPassant = false;

        Piece* epCaptured = 0;
        Piece* rook = 0;

        int rookFrom = -1;
        int rookTo = -1;

        if (moving->type() == 'K' && myAbs(dc - sc) == 2) {
            castle = true;
        }

        if (moving->type() == 'P' && dc != sc && dest == 0 && isEnPassantSquare(dr, dc, color)) {
            enPassant = true;
        }

        if (enPassant) {
            epCaptured = cells[epPawnRow][epPawnCol];
            cells[epPawnRow][epPawnCol] = 0;
        }

        cells[dr][dc] = moving;
        cells[sr][sc] = 0;

        if (castle) {
            if (dc == 6) {
                rookFrom = 7;
                rookTo = 5;
            }
            else {
                rookFrom = 0;
                rookTo = 3;
            }

            rook = cells[sr][rookFrom];
            cells[sr][rookTo] = rook;
            cells[sr][rookFrom] = 0;
        }

        bool check = isInCheck(color);

        if (castle) {
            cells[sr][rookFrom] = rook;
            cells[sr][rookTo] = 0;
        }

        cells[sr][sc] = moving;
        cells[dr][dc] = dest;

        if (enPassant) {
            cells[epPawnRow][epPawnCol] = epCaptured;
        }

        return check;
    }

    // Final check before allowing a move
    bool isLegalMoveForColor(int sr, int sc, int dr, int dc, char color) {
        if (!inside(sr, sc) || !inside(dr, dc)) return false;
        if (sr == dr && sc == dc) return false;
        if (cells[sr][sc] == 0) return false;
        if (cells[sr][sc]->getColor() != color) return false;
        if (hasOwn(dr, dc, color)) return false;
        if (cells[dr][dc] != 0 && cells[dr][dc]->type() == 'K') return false;
        if (!cells[sr][sc]->canMove(*this, sr, sc, dr, dc)) return false;
        if (wouldLeaveKingInCheck(sr, sc, dr, dc, color)) return false;

        return true;
    }

    // Checks if the player still has any legal move left
    bool hasAnyLegalMove(char color) {
        for (int sr = 0; sr < 8; sr++) {
            for (int sc = 0; sc < 8; sc++) {
                if (cells[sr][sc] == 0 || cells[sr][sc]->getColor() != color) {
                    continue;
                }

                for (int dr = 0; dr < 8; dr++) {
                    for (int dc = 0; dc < 8; dc++) {
                        if (isLegalMoveForColor(sr, sc, dr, dc, color)) {
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    bool isCheckmate(char color) {
        return isInCheck(color) && !hasAnyLegalMove(color);
    }

    bool isStalemate(char color) {
        return !isInCheck(color) && !hasAnyLegalMove(color);
    }

    // Prints the board and marks possible moves if needed
    void display(bool mark[8][8] = 0) const {
        setNormalColor();

        cout << "\n       a     b     c     d     e     f     g     h\n";
        cout << "    +-----+-----+-----+-----+-----+-----+-----+-----+\n";

        for (int i = 0; i < 8; i++) {
            setNormalColor();
            cout << " " << 8 - i << "  |";

            for (int j = 0; j < 8; j++) {
                setTileColor(i, j);

                if (mark != 0 && mark[i][j]) {
                    if (cells[i][j] == 0) {
                        cout << "  *  ";
                    }
                    else {
                        cout << " x" << cells[i][j]->symbol() << "  ";
                    }
                }
                else if (cells[i][j] != 0) {
                    cout << "  " << cells[i][j]->symbol() << "  ";
                }
                else {
                    cout << "     ";
                }

                setNormalColor();
                cout << "|";
            }

            cout << "  " << 8 - i << "\n";
            cout << "    +-----+-----+-----+-----+-----+-----+-----+-----+\n";
        }

        cout << "       a     b     c     d     e     f     g     h\n";

        cout << "\nWhite: ";
        cout << "\xE2\x99\x94" << " ";
        cout << "\xE2\x99\x95" << " ";
        cout << "\xE2\x99\x96" << " ";
        cout << "\xE2\x99\x97" << " ";
        cout << "\xE2\x99\x98" << " ";
        cout << "\xE2\x99\x99";

        cout << "    Black: ";
        cout << "\xE2\x99\x9A" << " ";
        cout << "\xE2\x99\x9B" << " ";
        cout << "\xE2\x99\x9C" << " ";
        cout << "\xE2\x99\x9D" << " ";
        cout << "\xE2\x99\x9E" << " ";
        cout << "\xE2\x99\x9F" << "\n";

        cout << "* = possible empty move, x = possible capture\n";

        setNormalColor();
    }

    // Shows all possible legal moves of the selected piece
    void showPossibleMoves(string from) {
        int sr, sc;

        if (!posToIndex(from, sr, sc)) {
            cout << "Invalid square. Example: e2\n";
            return;
        }

        if (cells[sr][sc] == 0) {
            cout << "No piece on " << from << ".\n";
            return;
        }

        if (cells[sr][sc]->getColor() != turn) {
            cout << "That is not " << colorName(turn) << "'s piece.\n";
            return;
        }

        bool mark[8][8];
        int count = 0;

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                mark[i][j] = false;
            }
        }

        for (int dr = 0; dr < 8; dr++) {
            for (int dc = 0; dc < 8; dc++) {
                if (isLegalMoveForColor(sr, sc, dr, dc, turn)) {
                    mark[dr][dc] = true;
                    count++;
                }
            }
        }

        display(mark);

        cout << "\nPossible moves for " << from << ": ";

        if (count == 0) {
            cout << "none";
        }
        else {
            for (int dr = 0; dr < 8; dr++) {
                for (int dc = 0; dc < 8; dc++) {
                    if (mark[dr][dc]) {
                        cout << indexToPos(dr, dc) << " ";
                    }
                }
            }
        }

        cout << "\n";
    }

    // Changes a pawn into another piece after reaching the last row
    void promotePawn(int r, int c) {
        if (cells[r][c] == 0 || cells[r][c]->type() != 'P') return;

        if (!((cells[r][c]->getColor() == 'W' && r == 0) || (cells[r][c]->getColor() == 'B' && r == 7))) {
            return;
        }

        char color = cells[r][c]->getColor();
        char choice;

        cout << "Promote pawn to Q, R, B, or N: ";
        cin >> choice;

        if (choice >= 'a' && choice <= 'z') {
            choice = choice - 32;
        }

        delete cells[r][c];

        if (choice == 'R') {
            cells[r][c] = new Rook(color);
        }
        else if (choice == 'B') {
            cells[r][c] = new Bishop(color);
        }
        else if (choice == 'N') {
            cells[r][c] = new Knight(color);
        }
        else {
            cells[r][c] = new Queen(color);
        }

        cells[r][c]->setMoved(true);
    }

    // Actually moves the piece after all rules have been checked
    void makeActualMove(int sr, int sc, int dr, int dc) {
        Piece* moving = cells[sr][sc];
        Piece* dest = cells[dr][dc];

        char color = moving->getColor();

        bool castle = false;
        bool enPassant = false;

        if (moving->type() == 'K' && myAbs(dc - sc) == 2) {
            castle = true;
        }

        if (moving->type() == 'P' && dc != sc && dest == 0 && isEnPassantSquare(dr, dc, color)) {
            enPassant = true;
        }

        if (enPassant) {
            delete cells[epPawnRow][epPawnCol];
            cells[epPawnRow][epPawnCol] = 0;
        }

        if (dest != 0) {
            delete dest;
        }

        cells[dr][dc] = moving;
        cells[sr][sc] = 0;
        moving->setMoved(true);

        // During castling, the rook also moves with the king
        if (castle) {
            if (dc == 6) {
                cells[sr][5] = cells[sr][7];
                cells[sr][7] = 0;
                cells[sr][5]->setMoved(true);
            }
            else {
                cells[sr][3] = cells[sr][0];
                cells[sr][0] = 0;
                cells[sr][3]->setMoved(true);
            }
        }

        // Old en passant data is cleared because it works for one turn only
        epRow = -1;
        epCol = -1;
        epPawnRow = -1;
        epPawnCol = -1;

        // If a pawn jumps two squares, its skipped square is saved
        if (moving->type() == 'P' && myAbs(dr - sr) == 2) {
            epRow = (sr + dr) / 2;
            epCol = sc;
            epPawnRow = dr;
            epPawnCol = dc;
        }

        promotePawn(dr, dc);

        turn = otherColor(turn);
    }

    // Takes input like e2 e4 and moves the piece if it is legal
    bool moveText(string from, string to) {
        int sr, sc, dr, dc;

        if (!posToIndex(from, sr, sc) || !posToIndex(to, dr, dc)) {
            cout << "Invalid input. Use positions like e2 e4.\n";
            return false;
        }

        if (!isLegalMoveForColor(sr, sc, dr, dc, turn)) {
            cout << "Illegal move. Use show " << from << " to see possible moves.\n";
            return false;
        }

        makeActualMove(sr, sc, dr, dc);
        return true;
    }
};

// Pawn has normal movement, first two-step move, diagonal capture, and en passant
bool Pawn::canMove(Board& board, int sr, int sc, int dr, int dc) const {
    int dir = 1;

    if (getColor() == 'W') {
        dir = -1;
    }

    if (dc == sc && dr == sr + dir && board.isEmpty(dr, dc)) {
        return true;
    }

    if (dc == sc && dr == sr + 2 * dir && !hasMoved()) {
        if (board.isEmpty(sr + dir, sc) && board.isEmpty(dr, dc)) {
            return true;
        }
    }

    if (dr == sr + dir && (dc == sc + 1 || dc == sc - 1)) {
        if (board.hasEnemy(dr, dc, getColor())) {
            return true;
        }

        if (board.isEnPassantSquare(dr, dc, getColor())) {
            return true;
        }
    }

    return false;
}

// Rook moves straight horizontally or vertically
bool Rook::canMove(Board& board, int sr, int sc, int dr, int dc) const {
    if (sr != dr && sc != dc) {
        return false;
    }

    return board.pathClear(sr, sc, dr, dc);
}

// Knight moves in L-shape and can jump over pieces
bool Knight::canMove(Board& board, int sr, int sc, int dr, int dc) const {
    int r = myAbs(dr - sr);
    int c = myAbs(dc - sc);

    return (r == 2 && c == 1) || (r == 1 && c == 2);
}

// Bishop moves diagonally
bool Bishop::canMove(Board& board, int sr, int sc, int dr, int dc) const {
    if (myAbs(dr - sr) != myAbs(dc - sc)) {
        return false;
    }

    return board.pathClear(sr, sc, dr, dc);
}

// Queen moves like both rook and bishop
bool Queen::canMove(Board& board, int sr, int sc, int dr, int dc) const {
    if (sr == dr || sc == dc) {
        return board.pathClear(sr, sc, dr, dc);
    }

    if (myAbs(dr - sr) == myAbs(dc - sc)) {
        return board.pathClear(sr, sc, dr, dc);
    }

    return false;
}

// King moves one step, and castling is also handled here
bool King::canMove(Board& board, int sr, int sc, int dr, int dc) const {
    if (myAbs(dr - sr) <= 1 && myAbs(dc - sc) <= 1) {
        return true;
    }

    if (dr == sr && myAbs(dc - sc) == 2) {
        return board.canCastle(getColor(), sr, sc, dr, dc);
    }

    return false;
}

// Game controls the main input and output flow
class Game {
private:
    Board board;
    bool gameOver;

public:
    Game() {
        gameOver = false;
    }

    // Shows the available commands
    void help() {
        cout << "\nCommands:\n";
        cout << "  show e2       : show possible moves of a piece represented with * ( possible moves ) and X (captures)\n";
        cout << "  e2 e4         : move a piece\n";
        cout << "  help          : show commands\n";
        cout << "  exit          : quit game\n";
    }

    // Main loop keeps the game running until it ends
    void start() {
        string first, from, to;

        while (!gameOver) {
            clearScreen();
            printTitle();

            board.display();

            cout << "\nTurn: " << board.colorName(board.getTurn()) << "\n";

            if (board.isInCheck(board.getTurn())) {
                cout << board.colorName(board.getTurn()) << " is in CHECK.\n";
            }

            cout << "Enter command: ";
            cin >> first;

            if (first == "exit") {
                gameOver = true;
            }
            else if (first == "help") {
                clearScreen();
                printTitle();
                help();
                pauseScreen();
            }
            else if (first == "show") {
                cin >> from;
                clearScreen();
                printTitle();
                board.showPossibleMoves(from);
                pauseScreen();
            }
            else if (first == "move") {
                cin >> from >> to;
                cout << "Do not use the move command. Use direct format like e2 e4.\n";
                pauseScreen();
            }
            else {
                from = first;
                cin >> to;
                playMove(from, to);
            }
        }

        setNormalColor();
    }

    // Plays one move and then checks if the game is over
    void playMove(string from, string to) {
        char before = board.getTurn();

        if (board.moveText(from, to)) {
            char now = board.getTurn();

            if (board.isCheckmate(now)) {
                clearScreen();
                printTitle();
                board.display();
                cout << "CHECKMATE! " << board.colorName(before) << " wins.\n";
                gameOver = true;
            }
            else if (board.isStalemate(now)) {
                clearScreen();
                printTitle();
                board.display();
                cout << "STALEMATE! Game drawn.\n";
                gameOver = true;
            }
        }
        else {
            pauseScreen();
        }
    }
};

int main() {
    // These lines help Windows terminal show chess symbols correctly
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Game game;
    game.start();

    setNormalColor();
    return 0;
}
#include <iostream>
#include <iomanip>
#include <string>

// Sudoku solver
class Board {
 public:
    // default ctor
    Board();
    friend std::istream& operator >>(std::istream& in, Board& b);
    friend std::ostream& operator <<(std::ostream& out, const Board& b);
     std::ostream& printSimple(std::ostream& out) const;
    bool solve();
    bool solved() {
        return unsolved == 0;
    } std::ostream& printDetailed(std::ostream& out) const;
 private:
    void doPairElimination(const int *nine, const int *start, const char *msg);
    void doOnlyInNine(const int *nine, const int *start, const char *msg);
    void nineElim(int index, const int *nine, int bitnum);
    void doSoleValues();
    char ch(int value, int bitnum) const;
    std::ostream& detailline(std::ostream& out, int index, int dline) const;
    static const int given = 0x8000;
    static const int calculated = 0x4000;
    static const int allnums = 0x03fe;

    int getbit(int square) const;
    int clrbit(int square, int bitnum);
    int getsquare(int index) const;
    int setsquare(int index, int value);
    int clrsquare(int index, int bitnum);
    int setcount(int index, int value);
    int getcount(int index) const;
    bool update(int index, int bitnum, int flags, const std::string = "");
    int onenum(int index, bool showcalc = true) const;

    // representation is as follows:
    // 15 : given
    // 14 : calculate
    //  9-1 : possible values
    //  0 : guess
    int brd[81];
    // array containing counts of remaining possibilities for each cell
    int counts[81];
    // to speed calculation, we make three static boards 
    // for rows, columns and subsquares, so that for any 
    // given index, all 9 related squares may be quickly
    // visited.
    const static int rows[81];
    const static int columns[81];
    const static int subsquares[81];
    const static int rstart[9];
    const static int cstart[9];
    const static int sstart[9];
    unsigned unsolved;
    bool verbose;
};

// here is how the board is laid out in memory
/*
     0,  1,  2,  3,  4,  5,  6,  7,  8,
     9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26,

    27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44,
    45, 46, 47, 48, 49, 50, 51, 52, 53,

    54, 55, 56, 57, 58, 59, 60, 61, 62,
    63, 64, 65, 66, 67, 68, 69, 70, 71,
    72, 73, 74, 75, 76, 77, 78, 79, 80
*/
// each array contains 9 starting values from which each [row,col,subsquare]
// may be visited
const int Board::rstart[] = { 0, 9, 18, 27, 36, 45, 54, 63, 72 };
const int Board::cstart[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
const int Board::sstart[] = { 0, 3, 6, 27, 30, 33, 54, 57, 60 };

const int Board::rows[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 0,
    10, 11, 12, 13, 14, 15, 16, 17, 9,
    19, 20, 21, 22, 23, 24, 25, 26, 18,

    28, 29, 30, 31, 32, 33, 34, 35, 27,
    37, 38, 39, 40, 41, 42, 43, 44, 36,
    46, 47, 48, 49, 50, 51, 52, 53, 45,

    55, 56, 57, 58, 59, 60, 61, 62, 54,
    64, 65, 66, 67, 68, 69, 70, 71, 63,
    73, 74, 75, 76, 77, 78, 79, 80, 72
};

const int Board::columns[] = {
    9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26,
    27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44,
    45, 46, 47, 48, 49, 50, 51, 52, 53,
    54, 55, 56, 57, 58, 59, 60, 61, 62,
    63, 64, 65, 66, 67, 68, 69, 70, 71,
    72, 73, 74, 75, 76, 77, 78, 79, 80,
    0, 1, 2, 3, 4, 5, 6, 7, 8
};

const int Board::subsquares[] = {
    1, 2, 9, 4, 5, 12, 7, 8, 15,
    10, 11, 18, 13, 14, 21, 16, 17, 24,
    19, 20, 0, 22, 23, 3, 25, 26, 6,

    28, 29, 36, 31, 32, 39, 34, 35, 42,
    37, 38, 45, 40, 41, 48, 43, 44, 51,
    46, 47, 27, 49, 50, 30, 52, 53, 33,

    55, 56, 63, 58, 59, 66, 61, 62, 69,
    64, 65, 72, 67, 68, 75, 70, 71, 78,
    73, 74, 54, 76, 77, 57, 79, 80, 60
};

Board::Board():unsolved(81), verbose(false) {
    for (int index = 0; index < 81; ++index) {
        setsquare(index, allnums);
        setcount(index, 9);
    }
}

std::istream& operator >>(std::istream& in, Board& b) {
    std::string line;

    for (int row = 0; row < 9; ++row) {
        std::getline(in, line);
        for (int col = 0; col < 9; ++col) {
            int ch = line[col] - '0';
            if ((ch >= 1) && (ch <= 9))
                b.update(row * 9 + col, ch, b.given);
        }
    }
    return in;
}

std::ostream& operator <<(std::ostream& out, const Board& b) {
    if (b.verbose)
        return b.printDetailed(out);
    return b.printSimple(out);
}

char Board::ch(int value, int bitnum) const {
    if (!(value & (1 << bitnum)))
        return '.';
    if (value & given) {
        return 'G';
    } else if (value & calculated) {
        return 'C';
    }
    return "0123456789"[bitnum];
}

std::ostream& Board::detailline(std::ostream& out, int index, int dline) const {
    int sq = getsquare(index);
    switch (dline) {
    case 0:
        out << ch(sq, 1) << ch(sq, 2) << ch(sq, 3) << ' ';
        break;
    case 1:
        out << ch(sq, 4) << ch(sq, 5) << ch(sq, 6) << ' ';
        break;
    case 2:
        out << ch(sq, 7) << ch(sq, 8) << ch(sq, 9) << ' ';
        break;
    default:
        out << "    ";
    }

    return out;
}

std::ostream& Board::printDetailed(std::ostream& out) const {
    int index = 0;
    for (int row = 0; row < 9; ++row) {
        for (int dline = 0; dline < 4; ++dline) {
            index = row * 9;
            for (int col = 0; col < 9; ++col, ++index) {
                detailline(out, index, dline);
                if (col % 3 == 2)
                    out << "  ";
            }
            out << std::endl;
        }
        if (row % 3 == 2)
            out << std::endl;
    }
    return out;
}

std::ostream& Board::printSimple(std::ostream& out) const {
    for (int index = 0; index < 81; ++index) {
        int sq = onenum(index);
        if (sq > 0)
            out << sq;
        else
            out << '.';
        if (index % 9 == (9 - 1))
            out << std::endl;
    }
    return out;
}

int Board::getbit(int square) const {
    for (int i = 1; i <= 9; i++)
        if (square & 1 << i)
            return i;
    return 0;
}

int Board::clrbit(int square, int bitnum) {
    if ((square & given) || (square & calculated))
        return square;
    else
        return square & ~(1 << bitnum);
}

int Board::getsquare(int index) const {
    return brd[index];
}

int Board::setsquare(int index, int value) {
    return brd[index] = value;
}

int Board::clrsquare(int index, int bitnum) {
    int sq = getsquare(index);
    int sq2 = setsquare(index, clrbit(sq, bitnum));
    if (sq != sq2)
        setcount(index, getcount(index) - 1);
    return sq2;
}

int Board::setcount(int index, int value) {
    return counts[index] = value;
}

int Board::getcount(int index) const {
    return counts[index];
}

/**
 * Given a cell index and bitnum, set the given bit and eliminate it
 * from all three associated nines.
 */
bool Board::update(int index, int bitnum, int flags, const std::string msg) {
    int sq = getsquare(index);
    // if this was not a candidate bit, reject the update
    if (!(sq & (1 << bitnum)))
        return false;
    --unsolved;
    if (verbose) {
        std::cout << "[" << index / 9 << ", " << index %
            9 << "]=" << bitnum << ' ';
        if (flags & given)
            std::cout << "G\n";
        else
            std::cout << "c (" << msg << ")\n";
    }
    // set just the one bit
    setsquare(index, (sq & ~allnums) | (1 << bitnum) | flags);
    counts[index] = 0;
    // clear the bit in each of the other squares by
    // row, column and subsquare
    nineElim(index, rows, bitnum);
    nineElim(index, columns, bitnum);
    nineElim(index, subsquares, bitnum);

    return true;
}

void Board::nineElim(int index, const int *nine, int bitnum) {
    for (int r = nine[index]; r != index; r = nine[r])
        clrsquare(r, bitnum);
}

int Board::onenum(int index, bool showcalc) const {
    int square = getsquare(index);
    if ((square & given) || (showcalc && (square & calculated)))
        return getbit(square);
    return 0;
}

void Board::doSoleValues() {
    bool more;
    do {
        more = false;
        for (int index = 0; index < 81; ++index) {
            if (1 == getcount(index)) {
                int sq = getsquare(index);
                update(index, getbit(sq), calculated, "doSoleValues");
                more = true;
            }
        }
    } while (more);
}

/**
 * For each given nine, if there is a pair of unsolved cells each
 * having exactly the same two remaining possibilities, then none of the 
 * other seven cells may have those numbers as possibilities.
 */
void Board::doPairElimination(const int *nine, const int *start, const char *msg) {
    for (int row = 0; row < 9; ++row) {
        for (int col = 0, i = start[row]; col < 9; ++col, i = nine[i]) {
            // does this cell have exactly 2 possibilities left?
            if (2 == getcount(i)) {
                // yes; see if there's an identical cell in this nine
                for (int j = nine[i], k = col + 1; k < 9; ++k, j = nine[j]) {
                    if (getsquare(i) == getsquare(j)) {
                        // so clear these bits in the other seven cells
                        // we do this using the nineElim call which 
                        // actually clears eight cells, but then we restore 
                        // the value.
                        if (verbose)
                            std::
                                cout << "clearing pair " << i << ", " << j <<
                                std::endl;
                        nineElim(i, nine, getbit(getsquare(i)));
                        nineElim(i, nine, getbit(getsquare(j)));
                        setsquare(j, getsquare(i));
                        setcount(j, 2);
                    }
                }
            }
        }
    }
}

/*
 * For each given Nine, if there is a square which contains, as a 
 * remaining possibility, the ONLY instance of a particular digit,
 * then that square must be assigned that digit.
 */
void Board::doOnlyInNine(const int *nine, const int *start, const char *msg) {
    int index = 0;
    bool more;

    do {
        more = false;
        /* 
         * The variable is labelled "row" but it's really just the index
         * into the particular square within the Nine.
         */
        for (int row = 0; row < 9; ++row) {
            index = start[row];
            /*
             * for each digit, count the number of squares that could 
             * still possibly contain it.
             */
            for (int bitnum = 1; bitnum <= 9; ++bitnum) {
                int count = 0;
                for (int col = 0, i = index; col < 9; ++col, i = nine[i]) {
                    int sq = getsquare(i);
                    if (!(sq & (given | calculated))) {
                        if (sq & (1 << bitnum))
                            ++count;
                    }
                }
                /*
                 * If only one square could possibly contain the digit,
                 * then set that square to be be the digit.
                 */
                if (count == 1) {
                    for (int col = 0, i = index; col < 9; ++col, i = nine[i]) {
                        int sq = getsquare(i);
                        if (sq & (1 << bitnum)) {
                            update(i, bitnum, calculated, msg);
                            if (verbose)
                                printDetailed(std::cout);
                            more = true;
                        }
                    }
                }
            }
        }
    } while (more);
}

bool Board::solve() {
    bool result = false;
    unsigned initial;
    /*
     * Continue working on the board until either:
     *   - the board is solved    OR
     *   - no progress was made in the last round
     */
    do {
        if (verbose)
            std::cout << "unsolved pr = " << unsolved << std::endl;
        doPairElimination(rows, rstart, "rowPairs");
        if (verbose)
            std::cout << "unsolved pc = " << unsolved << std::endl;
        doPairElimination(columns, cstart, "rowColumns");
        if (verbose)
            std::cout << "unsolved pq = " << unsolved << std::endl;
        doPairElimination(subsquares, sstart, "rowSubsquares");
        initial = unsolved;
        if (verbose)
            std::cout << "unsolved r = " << unsolved << std::endl;
        doOnlyInNine(rows, rstart, "doOnlyInRow");
        if (verbose)
            std::cout << "unsolved c = " << unsolved << std::endl;
        doOnlyInNine(columns, cstart, "doOnlyInCol");
        if (verbose)
            std::cout << "unsolved q = " << unsolved << std::endl;
        doOnlyInNine(subsquares, sstart, "doOnlyInSubsquares");
        if (verbose)
            std::cout << "unsolved s = " << unsolved << std::endl;
        doSoleValues();
    } while (unsolved && unsolved < initial);
    if (verbose)
        std::cout << "unsolved = " << unsolved << std::endl;
    return result;
}

int main() {
    Board b;

    std::cout << "Reading board from stdin\n";
    std::cin >> b;
    std::cout << b;
    b.solve();
    std::cout << "Writing board to stdout\n";
    std::cout << b;
    b.printDetailed(std::cout);
}

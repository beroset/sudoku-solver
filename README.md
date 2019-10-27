# sudoku-solver
I decided to implement a Sudoku solver using C++11, but without guessing.  That is, this Sudoku solver does *not* make guesses or backtracks, but instead only solves based on valid logical inference.  With that in mind, here is the first part of the file, including header includes and class declaration.

To speed things along, this version uses a number of static tables.  Sudoku uses three kinds of sets of nine squares.  They are rows, columns and subsquares.  Each square on the 9x9 grid belongs to exactly one of each of those.  The static tables work by allowing a generic routine to be able to step through all of the values in the associated subsquare.  For example, if we wanted to check the lower right square (number 80), we could check each item in its row by using the `rows[]` array.  Each entry contains the index of the next square to be visited, (in this case 72, which is the far left square on the last row).  

    / here is how the board is laid out in memory
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
    const int Board::rstart[] = { 0,  9, 18, 27, 36, 45, 54, 63, 72 };
    const int Board::cstart[] = { 0,  1,  2,  3,  4,  5,  6,  7,  8 };
    const int Board::sstart[] = { 0,  3,  6, 27, 30, 33, 54, 57, 60 };
    
    const int Board::rows[] = {
         1,  2,  3,  4,  5,  6,  7,  8,  0,
        10, 11, 12, 13, 14, 15, 16, 17,  9,
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
         0,  1,  2,  3,  4,  5,  6,  7,  8
    };
    
    const int Board::subsquares[] = {
         1,  2,  9,  4,  5, 12,  7,  8, 15,
        10, 11, 18, 13, 14, 21, 16, 17, 24,
        19, 20,  0, 22, 23,  3, 25, 26,  6,
    
        28, 29, 36, 31, 32, 39, 34, 35, 42,
        37, 38, 45, 40, 41, 48, 43, 44, 51,
        46, 47, 27, 49, 50, 30, 52, 53, 33,
    
        55, 56, 63, 58, 59, 66, 61, 62, 69,
        64, 65, 72, 67, 68, 75, 70, 71, 78,
        73, 74, 54, 76, 77, 57, 79, 80, 60
    };

As the code comments note, the main `solve()` routine simply applies various strategies until either the board is solved or no progress is made.

Using this code with the following board:

    .75..1..2
    ........9
    .9..27.4.
    ....943..
    .........
    ..381....
    .3.76..1.
    9........
    6..4..58.

yields the correct solution in 0.05 seconds on my laptop:

    875941632
    264583179
    391627845
    786294351
    159376428
    423815796
    538762914
    947158263
    612439587



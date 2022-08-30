#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <math.h>

// Helpful way to compile out debug code
#ifdef _DEBUG
#define D(x) x
#else
#define D(x)
#endif

// Standard sudoku board: 9x9.
//const int NROWS = 9, NCOLS = NROWS;
const int NROWS = 9;
const int NCOLS = 9;

/**
 * Use mmap to cleverly "read" the entire
 * contents of the file into memory.
 * It doesn't actually read it, just makes
 * a mapping, which is actually faster
 * than copying each byte!
 */
char* read_file(const char* const filename) {
    const int fd = open(filename, O_RDONLY);
    const size_t len = lseek(fd, 0, SEEK_END);
    char* buf = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if(buf == MAP_FAILED) {
    
        exit(EXIT_FAILURE);
    }

    return buf;
}

/**
 * Take the buffer and convert it to an array
 * of ints.
 *
 * NOTE: buf is non-const because
 * strtok eats it.
 *
 * NOTE: Making this a 2-d array is actually
 * really tricky. It requires a first row which
 * is full of pointers to subsequent rows (since
 * indexing is array[row][col]. It gets really messy,
 * especially if you want to treat it like a 1-d array.
 * I guess this is simply because the compiler doesn't
 * realize it's a contiguous block and cannot decay
 * the [][] into a single []. Oh well.
 */
int* create_board(const char* const buf) {
    const char delim[] = ",\n"; 	

    int *b = malloc(NROWS * NCOLS * sizeof(*b));

    int i = 0;

    char* const s = strdup(buf);
    char* token = strtok(s, (char*)delim);
    
    while(token) {
        // A "better" version of atoi
        const int val = strtol(token, NULL, 10);

        b[i++] = val;

        token = strtok(NULL, (char*)delim); // get next token
    }

    return b;
}

D(
void print_board(const int* const b) {
    printf("printing board-------------\n");
    for(int i = 0; i < NROWS; i++) {
        for(int j = 0; j < NCOLS; j++) {
            printf("%d ", b[i * NROWS + j]); 
        }
        printf("\n");
    }
    printf("---------------------------\n");
}
)

struct coord {
    bool seen;
    int r, c;
};

void check_if_seen(int val, int r, int c, struct coord* nums) {
    D(printf("(%d, %d)\t", r, c);)

    if(val == 0) { // 0 means empty
        D(printf("val: 0, skipping\n");)
        return; 
    }

    struct coord obj = nums[val];

    D(printf("val: %d,\tnums[val]: seen=%d, r=%d, c=%d\n", val, obj.seen,  obj.r, obj.c);)

    if(obj.seen) {
        // This is a duplicate!
        printf("Board invalid: (%d, %d) and (%d, %d) are both %d.\n",
                r, c, obj.r, obj.c, val);
        exit(EXIT_FAILURE);
    } // if
    // We've seen this number
    nums[val] = (struct coord) {.seen = true, .r = r, .c = c};
}

/**
 * Determine if the board is in a valid state.
 * Valid state is defined as no duplicate numbers in any
 * row, column, or 3x3 subgrid (of which there are 9).
 *
 * If the state is invalid, report the 
 * coords of duplicate numbers and terminate.
 */
void row_valid(const int* const board) {
    // First, check each row.
    // Use a list of bools to tell if it's in the set.
    struct coord nums[NCOLS+1]; // add 1 so that we can index by num, nums[0] is invalid

    for(int r = 0; r < NROWS; r++) {
        // Reset what we've seen in this row
        for(int i = 0; i < NCOLS+1; i++) 
            nums[i] = (struct coord) {.seen = false, .r = -1, .c = -1};

        // Pointer to this row (for convenience)
        const int* const row = board + r * NCOLS;

        // Check if there are any duplicates
        for(int c = 0; c < NCOLS; c++) {
            int val = row[c];
            check_if_seen(val, r, c, nums);
        } // for c
    } // for i

    // Next, check each column.
    // Next, check each square in the grid.
    printf("If you made it here, the rows must be valid.\n");
}

void col_valid(const int* const board) {
    // First, check each row.
    // Use a list of bools to tell if it's in the set.
    struct coord nums[NCOLS+1]; // add 1 so that we can index by num, nums[0] is invalid

    for(int c = 0; c < NCOLS; c++) {
        // Reset what we've seen in this row
        for(int i = 0; i < NCOLS+1; i++) 
            nums[i] = (struct coord) {.seen = false, .r = -1, .c = -1};


        // Check if there are any duplicates
        for(int r = 0; r < NROWS; r++) {
            // Pointer to this row (for convenience)
            const int* const row = board + r * NCOLS;
            int val = row[c];
            check_if_seen(val, r, c, nums);
        } // for c
    } // for i

    // Next, check each column.
    // Next, check each square in the grid.
    printf("If you made it here, the cols must be valid.\n");
}


void square_valid(const int* const board) {
    // Use a list of bools to tell if it's in the set.
    struct coord nums[NCOLS+1]; // add 1 so that we can index by num, nums[0] is invalid
    
    const int SUBSQUARE_LEN = (int)sqrt(NROWS);
    const int NUM_SUBSQUARES = NROWS / SUBSQUARE_LEN;

    for(int subsquare_row = 0; subsquare_row < NUM_SUBSQUARES; subsquare_row++) {
        const int min_row = SUBSQUARE_LEN * subsquare_row;
        const int max_row = min_row + SUBSQUARE_LEN;

        for(int subsquare_col = 0; subsquare_col < NUM_SUBSQUARES; subsquare_col++) {
            const int min_col = SUBSQUARE_LEN * subsquare_col;
            const int max_col = min_col + SUBSQUARE_LEN;

            // Traverse through each row in the subsquare rows.
            for(int r = min_row; r < max_row; r++) {
                for(int c = min_col; c < max_col; c++) {
                    const int* const row = board + r * subsquare_row * NCOLS;
                    int val = row[c * subsquare_col];
                    check_if_seen(val, r, c, nums);
                }
            }
        }
    }
    printf("If you made it here, the squares must be valid.\n");
}


int main(int argc, char** argv) {
    if(argc <= 1) {
        printf("Usage:\n\t./sudoku <filename>\n\n");
        return 1;
    }

	// Get string input from stdin
    const char* const buf = read_file(argv[1]);
    int* board = create_board(buf);
    // Copy 1-d board into 2-d thingy (TODO this may be no better than previous 2d board)
    //int board[NROWS][NCOLS];
    //memcpy(board, b, NROWS*NCOLS*sizeof(*b)); // don't trust!
                                              // TODO see if board[][] works (I doubt it will)
                                              // I don't think it does, just use int* for now

    D(print_board((const int* const)board);)

    // Is the board in a valid state?
    row_valid((const int* const) board); 
    col_valid((const int* const) board); 
    square_valid((const int* const) board); 

    // Cleanup
    munmap((char*)buf, sizeof(buf));
    free(board);
	return 0;
}


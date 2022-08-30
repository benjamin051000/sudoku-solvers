#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

// Helpful way to compile out debug code
#ifdef _DEBUG
#define D(x) x
#else
#define D(x)
#endif

// Standard sudoku board: 9x9.
//const int NROWS = 9, NCOLS = NROWS;
#define NROWS 9
#define NCOLS 9

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
    
    while(token != NULL) {
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
            const int idx = i * NROWS + j;
            printf("%d ", b[idx]); 
        }
        printf("\n");
    }
    printf("---------------------------\n");
}
)


int main(int argc, char** argv) {
    if(argc <= 1) {
        printf("Usage:\n\t./sudoku <filename>\n\n");
        return 1;
    }

	// Get string input from stdin
    const char* const buf = read_file(argv[1]);
    int* b = create_board(buf);
    // Copy 1-d board into 2-d thingy (TODO this may be no better than previous 2d board)
    int board[NROWS][NCOLS];
    memcpy(board, b, NROWS*NCOLS*sizeof(*b)); // don't trust!
                                              // TODO see if board[][] works (I doubt it will)

    D(print_board((const int* const)board);)

    // Cleanup
    munmap((char*)buf, sizeof(buf));
    free(b);
	return 0;
}


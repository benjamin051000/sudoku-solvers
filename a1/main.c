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

    if(buf == MAP_FAILED) {
        printf("Error: memory-mapping failed!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    return buf;
}

/**
 * Take the buffer and convert it to an array
 * of ints.
 */
int* create_board(const char* const buf) {
    char delim[] = ",\n"; 	

    // Standard 9x9 sudoku board.
    int *board = malloc(sizeof(int) * 9 * 9);

    int i = 0, j = 0;

    char* token = strtok(buf, delim);
    
    while(token) {
        // A "better" version of atoi
        const int val = strtol(token, NULL, 10);

        board[i++][j++] = val;

        strtok(buf, delim); // get next token
    }

    return board;
}


int main(int argc, char** argv) {
    if(argc <= 1) {
        printf("Usage:\n\t./sudoku <filename>\n\n");
        return 1;
    }

	// Get string input from stdin
    char* buf = read_file(argv[1]);

	printf("%s.\n", buf);

    free(buf);
	return 0;
}


# Sudoku Solver
## Usage
### Dependencies
This will work correctly on any system with the following packages installed:
- `make`
- `gcc`

### Build
```bash
make
```
### Run
You can run the project in two ways:
```bash
./sudoku <input file>
```

or, use `make` to run a default example (`sudoku1.txt`):
```bash
make run
```

The input file should be of the same format as `sudoku1.txt`, a 9x9 grid of comma-separated values, nothing else.

The program will return either a solved Sudoku board, or an error message explaining a conflict it found that made it impossible to solve the board.

### Clean
```bash
make clean
```

## The Project
### Assumptions
- The input filename will be passed in via a command-line argument rather than standard input (e.g., `./sudoku < ./sudoku1.txt`).
- In the event of a conflict, the program only prints the *first* conflict it encounters, not all conflicts.


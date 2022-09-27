#!/usr/bin/python3
from pprint import pprint
from sys import argv
import itertools

import numpy as np
from numpy.typing import NDArray
import z3

# Get input
try:
    filename = argv[1]
except IndexError:
    raise ValueError("Usage: python sudoku.py <input filename>")

with open(filename) as f:
    text = f.read().replace('\n', ',')
    
tiles = [n for n in text.split(',') if n]

matrix = np.array(tiles).reshape(9,9).astype(int)

# For each cell (9x9 cells total), create 9 variables that
# represent the presence (or absence) of that number.
# For example, x_3_4_5 means:
# row 3, column 5, value 5
# If it's true, this cell is 5.
# If it's false, it's a different number.
# Either all of the values in a cell will be false,
# Meaning the cell is empty,
# OR one of the values will be true,
# Meaning the cell is filled in with the true value.
grid = []
for i, j, k in itertools.product(range(9), repeat=3):
    grid.append(z3.Bool(f"x_{i}_{j}_{k}"))

grid = np.array(grid).reshape(9,9,9)


def exactly_one(arr: NDArray[z3.Bool]):
    """Given a 1-dimensional array-like of 
    z3 variables (e.g., z3.Bool), generate
    an exhaustive list of terms where one
    element is true and all others are false.
    Or them all together into one expression.
    """
    clauses = []
    for true_idx in range(len(arr)):
        clause = [
            e if i == true_idx  # One is true
            else z3.Not(e)  # The rest are false
            for i, e in enumerate(arr)
        ]
        # And all of these together
        clauses.append(z3.And(clause))

    # Any one of these terms can be true, so use Or
    return z3.Or(clauses)


# Create the solver object
solver = z3.Solver()

# Propositional Logic

# Every cell must have some number n
# Only one value per cell
for row in grid:
    for cell in row:
        solver.add(exactly_one(cell))


# If we have n in cell r,c, then it cannot appear in the same row
for row in grid:
    for num in range(9):
        # row is a 2d array
        # we need a 1d array of just num in this row
        num_in_row = row[:, num]
        solver.add(exactly_one(num_in_row))

# If we have n in cell r,c, then it cannot appear in the same column
for col in grid.T:
    for num in range(9):
        # col is 2d, get 1d of just num
        num_in_col = col[:, num]
        solver.add(exactly_one(num_in_col))

# If we have n in cell r,c, then it cannot appear in the same subsquare
for sub_x, sub_y in itertools.product(range(3), repeat=2):
    # Get the subsquare cells as an array
    # Should be easy enough with numpy!
    rmin, rmax = sub_x * 3, sub_x * 3 + 3
    cmin, cmax = sub_y * 3, sub_y * 3 + 3

    for num in range(9):
        num_in_sub = grid[rmin:rmax, cmin:cmax, num].flatten()
        #print(num_in_sub)
        #print('-'*50)
        solver.add(exactly_one(num_in_sub))


# Now, set all the ones we already have to True.
for r in range(9):
    for c in range(9):
        if (v := matrix[r][c]):
            # Set this boolean variable to True
            solver.add(grid[r][c][v-1] == True)

##################################################

if solver.check() == z3.unsat:
    # Board is invalid.
    print("Board is invalid!")
    exit()

model = solver.model()

def print_model():
    new_board = np.zeros((9,9))

    for r, c, v in itertools.product(range(9), repeat=3):
        if model.evaluate(grid[r][c][v]):
            new_board[r][c] = v

    # Print the final board
    for r in range(9):
        for c in range(9):
            print(f"{int(new_board[r][c])+1},", end="")

        print()


print_model()


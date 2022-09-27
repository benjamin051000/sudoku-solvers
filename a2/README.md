# Z3 Sudoku Solver
This is an implementation of a sudoku solver in Z3.

This uses the **Python** Z3 frontend.

## Install Dependencies
**NOTE: This project requires at least Python 3.8.**

First, install the Z3 library.
Next, install the Z3 Python library via `pip install z3-solver`.

## Usage
Run `python3 solver.py <filename.txt>`. Program will output either an error message stating the solver could not find a solution, or the solution in matrix form.

# Shortcomings
The only feature not implemented is showing the user where conflicting nodes are in an invalid matrix.

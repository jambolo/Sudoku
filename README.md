# Sudoku
Sudoku tools

## generate
Generates a puzzle.

#### Command syntax

    generate [<difficulty>]

#### Parameters

| Parameter  | Description |
|------------|-------------|
| difficulty | Desired number of empty cells (default: as many as possible) |

## profile
Finds the average time to generate and solve puzzles

    profile [<count>]

#### Parameters

| Parameter | Description |
|-----------|-------------|
| count     | Number of puzzles to profile (default: 1000) |

## rate
Rates the difficulty of a puzzle

#### Command syntax

    rate [-vv] <puzzle>

#### Parameters

| Parameter | Description |
|-----------|-------------|
| puzzle    | 81 digits, 0-9 |

#### Options

| Option  | Description |
|---------|-------------|
| -v, -vv | Verbosity   |

#### Difficulty computation

The overall difficulty is computed as follows:

    for each difficulty
        add the difficulty factor, 2 ** (d + (1 - 1/n)/2)
    return log2 of the result - 1

The result is the difficulty factor of the most difficult step plus up to +0.5 for additional steps of that difficulty, plus up to 0.5 for for lower difficulty steps.

## solve
Solves a puzzle

#### Command syntax

    solve <puzzle>

#### Parameters

| Parameter | Description |
|-----------|-------------|
| puzzle    | 81 digits, 0-9 |

## suggest
Suggests the next move for solving a puzzle

#### Command syntax

    suggest [-a] [-vvv | -j] <puzzle>

#### Parameters

| Parameter | Description |
|-----------|-------------|
| puzzle    | 81 digits, 0-9 |

#### Options

| Option        | Description |
|---------------|-------------|
| -v, -vv, -vvv | Verbosity   |
| -a            | outputs the all of the steps for a complete solution |
| -j            | outputs in JSON format |

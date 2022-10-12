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

The overall difficulty is
$$D \left( 1 + {1 \over 2}{{n_D - 1} \over n_D + 1} + {1 \over 2} \sum_{d=1}^{D-1} {{n_d \over {n_d+1}} {1 \over 2^{D-d}}} \right)$$
where _K_ is the highest difficulty technique, _d_ is the difficulty of technique _i_ and _n_ is the number of times
technique _i_ is used.

The result is the difficulty factor of the most difficult technique plus up to +0.5 for additional steps of that
technique, plus up to +0.5 for for lower difficulty techniques.

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

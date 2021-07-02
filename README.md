# KenKen solver

This is a (somewhat optimized) backtracking solver in C++11 for [KenKen](https://en.wikipedia.org/wiki/KenKen) puzzles, also known as Calcudoku. It is licensed under the [ISC license](LICENSE.txt).

The puzzle input is read from a file (path given as argument) and its solution is printed to `stdout`. No validity or uniqueness checks are performed. The format is easily determined from the available [examples](res), noting that cages with the subtraction or division operator *must* be of size 2.

Timing information is printed to `stderr`. Only the time spent backtracking is measured; I/O is ignored, for example. On my high-end machine from 2014, the running time ranges from [<1μs](res/kenkenpuzzle-expert-S.ken) to [~2.2s](res/menneske9-4474.ken). 'Average' puzzles (6x6) are solved in [≪1ms](res/menneske6-74285.ken).


## Some implementation details

* Each cage stores their intermediate result (`current`), i.e. the result of the calculation involving all non-empty cells. This way, new cell values can be tested more efficiently, without checking other cage cells.
* Each column, row, and cage keeps a list of candidates, i.e. values that are not proven to be illegal. This list is stored as a bitset (a simple `int` suffices) where the `i`th bit is set if `i` is a possible value. This allows us to take intersections and find the 'next' candidate in O(1) time, using the `&` operator and `__builtin_ctz` intrinsic (count trailing zeros) respectively.
* Using [candidates.py](candidates.py), bitsets for all cage configurations of size ≤5 are precomputed (larger cages are rare and therefore not worth the additional space). Here, a grid size of 9 and the 'worst' cage shape (i.e. a staircase, which permits the most duplicates) is assumed. For smaller grids, illegal candidates are simply ignored. This is sometimes suboptimal (e.g. in a `*6` cage of size 2, the value 1 is possible in a grid of size 9, but not of size 3), but grids of size ≤8 are solved almost instantly anyway.

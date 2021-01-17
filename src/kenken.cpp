#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#define abs(x) ((x)<0?-(x):(x))

typedef std::chrono::high_resolution_clock hr_clock;
typedef std::chrono::milliseconds ms;

struct cage {
    int op; // 0 = add, 1 = sub, 2 = mul, 3 = div
    int target;
    std::vector<int> cells; // i = x + y * size  where  (0,0) is upper left
};

int size;
int grid[9*9];
cage* cageOf[9*9];

// The following arrays contain lists of candidates for every row and column.
// For row y and value n, the next valid candidate is n + rowCandidates[y][n],
// if less than or equal to size. To remove a candidate, add rowCandidates[y][n]
// to rowCandidates[y][nPrev], where nPrev is the previous value from the
// iteration, i.e. n == nPrev + rowCandidates[y][nPrev]. To re-insert a value,
// subtract rowCandidates[y][n] from rowCandidates[y][nPrev], reversing the
// change. This allows O(n) iteration and O(1) insertion/deletion, similar to a
// linked list. However, this solution (hopefully) performs better due to its
// simplicity and locality of reference.
int colCandidates[9][10];
int rowCandidates[9][10];

bool valid(int i, int n) {
    cage* c = cageOf[i];
    bool full = true;
    for (int j : c->cells)
        if (j > i) {
            full = false; // j comes after i, so still empty
        } else if (j < i) {
            switch (c->op) {
                case 0:
                    n += grid[j];
                    break;
                case 1:
                    n = abs(grid[j] - n);
                    break;
                case 2:
                    n *= grid[j];
                    break;
                case 3:
                    int max = n, min = grid[j];
                    if (max < min) std::swap(max, min);
                    if (max % min != 0) return false;
                    n = max / min;
            }
        }

    if (full) return n == c->target;
    if (c->op == 0) return n < c->target;
    if (c->op == 2) return n <= c->target; // missing number could be 1

    return true; // otherwise
}

bool backtrack(int i) {
    if (i == size*size) return true;

    int y = i / size;
    int* cc = colCandidates[i - size * y];
    int* rc = rowCandidates[y];

    for (int n = 0, m = 0, nPrev = 0, mPrev = 0; true; ) {
        // 1. find next candidate
        do {
            if (n <= m) {
                nPrev = n, n += cc[n];
                if (n > size) return false;
            } else {
                mPrev = m, m += rc[m];
                if (m > size) return false;
            }
        } while (n != m);

        // 2. recur (if cage is valid)
        if (valid(i, n)) {
            grid[i] = n;
            cc[nPrev] += cc[n];
            rc[mPrev] += rc[m];

            if (backtrack(i+1)) return true;

         // grid[i] = 0;
            cc[nPrev] -= cc[n];
            rc[mPrev] -= rc[m];
        }
    }
}

int main(int argc, char** argv) {
    std::ifstream input(argv[1]);
    std::string line;

    // 1. process first line, containing size
    std::getline(input, line);
    size = line[0] - '0';

    // 2. process other lines, containing cages
    while (std::getline(input, line)) {
        cage* c = new cage;

        switch (line[0]) {
            case '+': c->op = 0; break;
            case '-': c->op = 1; break;
            case '*': c->op = 2; break;
            case '/': c->op = 3;
        }

        int i = 0;
        c->target = 0;
        while (line[++i] != ' ') {
            c->target *= 10;
            c->target += line[i] - '0';
        }

        for (i++; i < line.size(); i += 3) {
            int k = (line[i] - 'a') + (line[i+1] - '1') * size; // like chess
            c->cells.push_back(k);
            cageOf[k] = c;
        }
    }

    // 3. initialize remaining data structures
    std::memset(grid, 0, sizeof(int) * size * size);
    for (int i = 0; i < size; i++)
        for (int j = 0; j <= size; j++)
            colCandidates[i][j] = rowCandidates[i][j] = 1;

    // 4. run & profit!
    auto time_start = hr_clock::now();
    bool success = backtrack(0);
    auto time_end = hr_clock::now();

    std::cout
            << (success ? "success!" : "(no solution)")
            << "  in "
            << std::chrono::duration_cast<ms>(time_end - time_start).count()
            << " ms\n";

    if (success)
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++)
                std::cout << grid[x + y * size] << ' ';
            std::cout << '\n';
        }
}

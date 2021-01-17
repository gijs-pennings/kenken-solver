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

bool valid(int i, int n) {
    // 1. check column
    for (int j = i % size; j < i; j += size)
        if (grid[j] == n)
            return false;

    // 2. check row
    for (int j = i / size * size; j < i; j++)
        if (grid[j] == n)
            return false;

    // 3. check cage (note: we reuse n -- bad style, but whatever)
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
    for (int n = 1; n <= size; n++)
        if (valid(i, n)) {
            grid[i] = n;
            if (backtrack(i+1)) return true;
            // grid[i] = 0;
        }
    return false;
}

int main(int argc, char** argv) {
    std::ifstream input(argv[1]);
    std::string line;

    // 1. process first line, containing size
    std::getline(input, line);
    size = line[0] - '0';

    std::memset(grid, 0, sizeof(int) * size * size);

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

    // 3. run & profit!
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

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include <candidates.h>

#define abs(x) ((x)<0?-(x):(x))

typedef std::chrono::high_resolution_clock hr_clock;
typedef std::chrono::milliseconds ms;

struct cage {
    int op; // 0 = add, 1 = sub, 2 = mul, 3 = div
    int target;
    int candidates; // bitset

    int current; // initially identity of operator
    int numEmpty;

    bool valid(int n) {
        switch (op) {
            case 0:
                n += current; break;
            case 1:
                n = abs(n - current); break;
            case 2:
                n *= current; break;
            case 3:
                int max = n, min = current;
                if (max < min) std::swap(max, min);
                if (max % min != 0) return false;
                n = max / min;
        }

        if (numEmpty == 1) {
            if (n != target) return false; // since all cells are filled
        } else if (op == 0) {
            if (n >= target) return false;
        } else if (op == 2) {
            if (n > target) return false; // missing number could be 1!
        }

        // only update fields if new cell value is valid
        current = n;
        numEmpty--;

        return true;
    }
};

int size;
int grid[9*9];
cage* cageOf[9*9];

int colCandidates[9];
int rowCandidates[9];

bool backtrack(int i) {
    if (i == size*size) return true;

    cage *c = cageOf[i];
    int original = c->current;

    int y = i / size;
    int x = i - size*y;

    for (int mask, set = colCandidates[x] & rowCandidates[y] & c->candidates;
            set != 0; set ^= mask) {
        // 1. determine next candidate
        int n = __builtin_ctz(set);
        mask = 1 << n;

        // 2. check cage validity & recur if OK
        if (c->valid(n)) {
            colCandidates[x] ^= mask;
            rowCandidates[y] ^= mask;

            if (backtrack(i+1)) { grid[i] = n; return true; }

            c->current = original;
            c->numEmpty++;
            colCandidates[x] ^= mask;
            rowCandidates[y] ^= mask;
        }
    }

    return false;
}

int main(int argc, char** argv) {
    std::ifstream input(argv[1]);
    std::string line;

    // 1. process first line, containing size
    std::getline(input, line);
    size = line[0] - '0';
    int mask = (1 << size+1) - 2; // e.g. for size 3 we get 0b1110

    // 2. process other lines, containing cages
    while (std::getline(input, line)) {
        cage* c = new cage(); // zero initialization

        int i = 0;
        while (line[++i] != ' ') {
            c->target *= 10;
            c->target += line[i] - '0';
        }

        for (i++; i < line.size(); i += 3) {
            c->numEmpty++;
            cageOf[(line[i] - 'a') + (line[i+1] - '1') * size] = c;
        }

        switch (line[0]) {
            case '+':
             // c->op = 0;
                c->candidates = c->numEmpty > candidates::MAX_CAGE
                        ? mask : candidates::add[c->numEmpty][c->target] & mask;
             // c->current = 0;
                break;
            case '-':
                c->op = 1;
                c->candidates = candidates::sub[c->target] & mask;
             // c->current = 0;
                break;
            case '*':
                c->op = 2;
                c->candidates = c->numEmpty > candidates::MAX_CAGE
                        ? mask : candidates::mul[c->numEmpty][c->target] & mask;
                c->current = 1;
                break;
            case '/':
                c->op = 3;
                c->candidates = candidates::div[c->target] & mask;
                c->current = 1;
        }
    }

    // 3. initialize remaining data structures
    for (int i = 0; i < size; i++) colCandidates[i] = rowCandidates[i] = mask;

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
                std::cout << grid[x + y*size] << ' ';
            std::cout << '\n';
        }
}

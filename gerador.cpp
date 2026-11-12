//
// Problem generator for ASA Project 26/27
//
// Usage: ./gerador <num_blocks> <max_dimension> [seed]
//

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

using namespace std;

using Block = array<int, 3>;

static bool parseInt(const char* s, int& out)
{
    string str(s);
    if(str.empty() || str.find_first_not_of("0123456789") != string::npos)
        return false;
    try {
        out = stoi(str);
    } catch(...) {
        return false;
    }
    return true;
}

static unsigned long long distinctBlocks(unsigned long long d)
{
    if(d > 1000000ULL)
        return 0;
    // d(d+1)(d+2)/6 <= ~1.7e17 for d = 1e6, fits in 64 bits
    return d * (d + 1) * (d + 2) / 6;
}

int main(int argc, char* argv[])
{
    if(argc < 3 || argc > 4) {
        cerr << "Usage: " << argv[0] << " <num_blocks> <max_dimension> [seed]\n";
        return 1;
    }

    int n, maxDim, seed;
    if(!parseInt(argv[1], n) || !parseInt(argv[2], maxDim) || n == 0 ||
		maxDim == 0) {
        cerr << "Error: both num_blocks and max_dimension must be positive"
			 << "integers.\n";
        return 1;
    }

    mt19937_64 rng;
    if(argc == 4) {
        if(!parseInt(argv[3], seed)) {
            cerr << "Error: seed must be a non-negative integer.\n";
            return 1;
        }
        rng.seed(seed);
    } else {
        rng.seed(random_device{}());
    }

    const unsigned long long total = distinctBlocks(maxDim);  // 0 = unbounded
    if(total != 0 && n > total) {
        cerr << "Error: only " << total << " distinct blocks exist with "
             << "dimensions in [1, " << maxDim << "], but " << n
             << " were requested.\n";
        return 1;
    }

    vector<Block> blocks;
    blocks.reserve(n);

    if(total != 0 && n * 2 > total) {
        // Dense request: enumerate every distinct block, shuffle, take n.
        // (total <= 2n here, so memory is proportional to the output.)
        vector<Block> all;
        all.reserve(total);
        for(int a = 1; a <= maxDim; a++)
            for(int b = a; b <= maxDim; b++)
                for(int c = b; c <= maxDim; c++)
                    all.push_back({a, b, c});
        shuffle(all.begin(), all.end(), rng);
        blocks.assign(all.begin(), all.begin() + n);
    } else {
        // Sparse request: rejection sampling (expected < 2 tries per block).
        uniform_int_distribution<int> dist(1, maxDim);
        set<Block> seen;
        while(blocks.size() < n) {
            Block k = {dist(rng), dist(rng), dist(rng)};
            sort(k.begin(), k.end());
            if(seen.insert(k).second)
                blocks.push_back(k);
        }
    }

    // Print, randomizing the order of the dimensions inside each block so the
    // solver is exercised with all orientations.
    cout << n << "\n";
    for(Block& k : blocks) {
        shuffle(k.begin(), k.end(), rng);
        cout << k[0] << " " << k[1] << " " << k[2] << "\n";
    }
    return 0;
}
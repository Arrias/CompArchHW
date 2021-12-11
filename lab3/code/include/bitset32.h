#ifndef CODE_BITSET32_H
#define CODE_BITSET32_H

#define BITSET_32_SIZE 32

#include <inttypes.h>
#include <utility>
#include <vector>
#include <algorithm>

using std::pair;
using std::vector;
using std::max;

struct bitset32 {
private:
    uint32_t bits, size;

public:
    bitset32() : size(0), bits(0) {}

    bitset32(uint32_t x, uint32_t sz = BITSET_32_SIZE);

    bitset32(const vector<pair<bool, int>> &vec);

    uint32_t get_bits() const;

    uint32_t get_size() const;

    bitset32 operator&(bitset32 other);

    void addBit(bool bit);

    bool operator[](int id) const;

    void addBits(bitset32 other);

    bitset32 substr(int l, int r) const;

    void setBit(uint32_t id, bool bit);
};

bool operator==(const bitset32 &l, const bitset32 &r);

#endif //CODE_BITSET32_H

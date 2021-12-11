#include "bitset32.h"

bitset32::bitset32(uint32_t x, uint32_t sz) : size(sz), bits(x) {}

bitset32::bitset32(const vector<pair<bool, int>> &vec) {
    size = BITSET_32_SIZE;
    for (auto &[bit, pos]: vec) {
        setBit(pos, bit);
    }
}

uint32_t bitset32::get_bits() const {
    return bits;
}

uint32_t bitset32::get_size() const {
    return size;
}

bitset32 bitset32::operator&(bitset32 other) {
    bitset32 ret(bits & other.bits, max(other.get_size(), this->get_size()));
    return ret;
}

void bitset32::addBit(bool bit) {
    bits += (1 << size++) * bit;
}

bool bitset32::operator[](int id) const {
    return ((bits >> id) & 1);
}

void bitset32::addBits(bitset32 other) {
    for (int i = 0; i < other.size; ++i) {
        addBit(other[i]);
    }
}

bitset32 bitset32::substr(int l, int r) const {
    bitset32 ret;
    for (int i = l; i <= r; ++i) {
        ret.addBits(this->operator[](i));
    }
    return ret;
}

void bitset32::setBit(uint32_t id, bool bit) {
    size = max(size, id + 1);
    bits |= (1 << id);
    if (!bit) {
        bits ^= (1 << id);
    }
}

bool operator==(const bitset32 &l, const bitset32 &r) {
    return l.get_bits() == r.get_bits();
}














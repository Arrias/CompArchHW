#ifndef CODE_STREAM_H
#define CODE_STREAM_H

#include "bitset32.h"

template<class T>
struct Stream {
    T ptr, end_ptr;
    uint8_t bit = 0;

    Stream(T ptr, T end_ptr) : ptr(ptr), end_ptr(end_ptr) {}

    bool exist() const {
        return ptr != end_ptr;
    }

    bool getNextBit() {
        bool ret = (((*ptr) >> bit) & 1);
        if (++bit == 8) {
            ++ptr;
            bit = 0;
        }
        return ret;
    }

    bitset32 takeN(int n) {
        bitset32 bits;
        while (n--) {
            bits.addBit(getNextBit());
        }
        return bits;
    }
};

#endif //CODE_STREAM_H
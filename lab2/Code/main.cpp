#include <iostream>
#include <iomanip>
#include "Cache/Cache.h"
#include "Operative/Operative.h"

const unsigned L1_associativity = 4;
const unsigned L1_volume = 32 * 1024;
const unsigned L1_row_size = 64;

const unsigned L2_associativity = 8;
const unsigned L2_volume = 1024 * 1024;
const unsigned L2_row_size = 64;

Operative operative; // содержит 6 массивов из объявления
Cache L2(L2_associativity, L2_volume, L2_row_size, &operative);
Cache L1(L1_associativity, L1_volume, L1_row_size, &L2);

address_type getAddress(unsigned mas_id, unsigned elem_id) {
    return (1ull) * sizeof(double) * (mas_id * size + elem_id);
}

double getValue(unsigned mas_id, unsigned elem_id) {
    return L1.getValue(getAddress(mas_id, elem_id));
}

void setValue(unsigned mas_id, unsigned elem_id, double value) {
    L1.setValue(getAddress(mas_id, elem_id), value);
}

enum ids {
    x, y, z, xx, yy, zz
};

// тестируемая функция
void f(double w) {
    for (unsigned int i = 0; i < size; ++i) {
        double temp = getValue(xx, i) * w + getValue(x, i);
        setValue(x, i, temp);
    }

    for (unsigned int i = 0; i < size; ++i) {
        double temp = getValue(yy, i) * w + getValue(y, i);
        setValue(y, i, temp);
    }

    for (unsigned int i = 0; i < size; ++i) {
        double temp = getValue(zz, i) * w + getValue(z, i);
        setValue(z, i, temp);
    }
}

int main() {
    std::cout.setf(std::ios::fixed);
    std::cout.precision(10);

    double w = 239.0;
    f(w);

    std::cout << "L1's percent is: " << L1.getPercent() << std::endl;
    std::cout << "L2's percent is: " << L2.getPercent() << std::endl;

    L1.reset_cache();
    L2.reset_cache();
    return 0;
}

#include "Operative.h"
#include <cstdlib>

void Operative::takePosition(address_type address, unsigned int &i, unsigned int &j) {
    i = address / (size * sizeof(double));
    j = (address % (size * sizeof(double)) / sizeof(double));
}

double Operative::getValue(address_type address) {
    unsigned i, j;
    takePosition(address, i, j);
    return ar[i][j];
}

void Operative::setValue(address_type address, double value) {
    unsigned i, j;
    takePosition(address, i, j);
    ar[i][j] = value;
}

/* Заполним случайными данными */
Operative::Operative() {
    for (unsigned i = 0; i < cnt; ++i) {
        for (unsigned j = 0; j < size; ++j) {
            ar[i][j] = rand();
        }
    }
}

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

std::vector<double> Operative::getRow(address_type address) {
    std::vector<double> ret(8);
    unsigned i, j;
    takePosition(address, i, j);
    for (int k = j; k < j + ret.size(); ++k)
        ret[k - j] = ar[i][k];
    return ret;
}

void Operative::setRow(address_type address, std::vector<double> new_row) {
    unsigned i, j;
    takePosition(address, i, j);
    for (int k = j; k < j + new_row.size(); ++k) {
        ar[i][k] = new_row[k - j];
    }
}

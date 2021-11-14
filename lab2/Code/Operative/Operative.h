#pragma once

#include "../Loader/Loader.h"

const unsigned long long size = 1024 * 1024;
const unsigned long long cnt = 6;

struct Operative : public Loader {
    /* {x, y, z, xx, yy, zz} */
    double ar[cnt][size];

    void takePosition(address_type address, unsigned &i, unsigned &j);

    double getValue(address_type address) override;

    void setValue(address_type address, double value) override;

    std::vector<double> getRow(address_type address) override;

    void setRow(address_type address, std::vector<double> new_row) override;

    Operative();
};



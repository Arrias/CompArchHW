#pragma once
#include <vector>

using address_type = unsigned long long;

struct Loader {
    virtual double getValue(address_type address) = 0;

    virtual void setValue(address_type address, double value) = 0;

    virtual std::vector<double> getRow(address_type address) = 0;

    virtual void setRow(address_type address, std::vector<double> new_row) = 0;
};



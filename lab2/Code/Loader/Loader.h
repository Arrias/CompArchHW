#pragma once

using address_type = unsigned long long;

struct Loader {
    virtual double getValue(address_type address) = 0;

    virtual void setValue(address_type address, double value) = 0;
};



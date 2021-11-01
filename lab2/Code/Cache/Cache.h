#pragma once

#include "../Loader/Loader.h"
#include <vector>

class Cache : public Loader {
    struct Row {
        std::vector<double> values;
        bool is_loaded;
        unsigned old;
        address_type row_tag;

        Row(unsigned sz) : is_loaded(false), old(0), row_tag(0) {
            values.resize(sz);
        }
    };

    unsigned total_requests = 0;
    unsigned total_misses = 0;
    unsigned row_size;
    unsigned way_size;

    std::vector<Row> data;

    /* Если значение не нашлось здесь, то запрашиваем у следующего кэша, либо у опер. памяти */
    Loader *next_loader;

public:
    Cache(unsigned associativity, unsigned sz, unsigned row_size, Loader *next_loader);

    void resetRow(unsigned row);

    /* (был ли элемент в кэше, позиция) */
    std::pair<bool, int> haveValue(address_type address);

    double getValue(address_type address) override;

    void setValue(address_type address, double value) override;

    // save to next_memory
    void reset_cache();

    double getPercent();
};



#include "Cache.h"

Cache::Cache(unsigned int associativity, unsigned int sz, unsigned int row_size, Loader *next_loader) : row_size(row_size),
                                                                                                        next_loader(next_loader) {
    unsigned rows = sz / row_size;
    way_size = rows / associativity;
    data.assign(rows, Row(row_size / sizeof(double)));
}

void Cache::resetRow(unsigned int row) {
    address_type full_address = (data[row].row_tag * way_size + (row % way_size)) * row_size;
    next_loader->setRow(full_address, data[row].values);
}

std::pair<bool, int> Cache::haveValue(address_type address) {
    address /= row_size;
    unsigned to_row = address & (way_size - 1);
    unsigned address_tag = address / way_size;

    for (unsigned i = to_row; i < data.size(); i += way_size) {
        if (data[i].is_loaded && data[i].row_tag == address_tag) {
            return std::make_pair(true, i);
        }
    }

    unsigned oldest_row = to_row;
    for (unsigned i = to_row; i < data.size(); i += way_size) {
        if (data[i].old < data[oldest_row].old) {
            oldest_row = i;
        }
    }

    if (data[oldest_row].is_loaded) { // сбросить строку
        resetRow(oldest_row);
    }

    data[oldest_row].is_loaded = true;
    data[oldest_row].old = total_requests;
    data[oldest_row].row_tag = address_tag;
    address_type full_address = address * row_size;

    auto new_mem = next_loader->getRow(full_address);
    for (int i = 0; i < data[oldest_row].values.size(); ++i) {
        data[oldest_row].values[i] = new_mem[i];
    }
    return std::make_pair(false, oldest_row);
}

double Cache::getValue(address_type address) {
    total_requests++;
    unsigned number = (address & (row_size - 1)) / sizeof(double);
    auto hv = haveValue(address);
    total_misses += !hv.first;

    data[hv.second].old = total_requests;
    return data[hv.second].values[number];
}

void Cache::setValue(address_type address, double value) {
    ++total_requests;
    unsigned number = (address & (row_size - 1)) / sizeof(double);
    auto hv = haveValue(address);
    total_misses += !hv.first;

    data[hv.second].old = total_requests;
    data[hv.second].values[number] = value;
}

void Cache::reset_cache() {
    for (int i = 0; i < data.size(); ++i) {
        if (!data[i].is_loaded) continue;
        resetRow(i);
    }
}

double Cache::getPercent() {
    if (!total_requests) return 100.0;
    return 100.0 * (double) (total_requests - total_misses) / total_requests;
}

std::vector<double> Cache::getRow(address_type address) {
    ++total_requests;
    auto hv = haveValue(address);
    total_misses += !hv.first;

    return data[hv.second].values;
}

void Cache::setRow(address_type address, std::vector<double> new_row) {
    ++total_requests;
    auto hv = haveValue(address);

    total_misses += !hv.first;
    data[hv.second].values = new_row;
}





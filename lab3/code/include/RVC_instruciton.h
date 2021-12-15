//
// Created by arrias on 12.12.2021.
//

#ifndef CODE_RVC_INSTRUCITON_H
#define CODE_RVC_INSTRUCITON_H

#include "constants.h"
#include "bitset32.h"
#include <utility>

using std::vector;
using std::string;
using std::pair;

struct Check {
    vector<int> inds;
    vector<bool> vals;
};

enum ArgType {
    reg,
    other,
};

enum OutFormat {
    with_offset,
    no_offest
};

struct Argument {
    vector<pair<int32_t, int32_t>> bit_positions;
    ArgType arg_type;
    OutFormat out_format;

    Argument(const vector<pair<int32_t, int32_t>> &bit_positions, ArgType arg_type) :
            bit_positions(bit_positions), arg_type(arg_type) {}

    void extract_value_and_print(bitset32 mask, FILE *out) {
        bitset32 result;
        for (auto &[pos1, pos2]: bit_positions) {
            result.setBit(pos2, mask[pos1]);
        }
        if (arg_type == reg) {
            fprintf(out, "%s", registers_names[result.get_bits()].c_str());
        } else {
            int32_t res = result.get_bits();
            if (result[result.get_size() - 1]) {
                res -= (1 << (result.get_size()));
            }
            fprintf(out, "%d", res);
        }
    }
};

struct RV32_instrucion {
    string name;
    bitset32 equal_mask;
    bitset32 and_mask;
    OutFormat out_format;

    vector<Argument> arguments;

    RV32_instrucion(string name, const Check checks, const vector<Argument> arguments, OutFormat out_format = no_offest) : name(name),
                                                                                                                           arguments(
                                                                                                                                   arguments),
                                                                                                                           out_format(
                                                                                                                                   out_format) {
        for (int i = 0; i < checks.inds.size(); ++i) {
            equal_mask.setBit(checks.inds[i], checks.vals[i]);
            and_mask.setBit(checks.inds[i], true);
        }
    }

    bool accept(bitset32 msk) {
        return (msk & and_mask) == equal_mask;
    }

    void extract_args(bitset32 row, FILE *out) {
        bool isFirst = true;
        int it = 0;

        for (auto &argument: arguments) {
            if (it == arguments.size() - 1 && out_format == with_offset) fprintf(out, "(");
            else if (!isFirst) fprintf(out, ", ");
            argument.extract_value_and_print(row, out);
            isFirst &= false;
            ++it;
        }

        if (out_format == with_offset) fprintf(out, ")");
    }
};



#endif //CODE_RVC_INSTRUCITON_H

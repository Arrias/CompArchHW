#ifndef CODE_RV32_INSTRUCTION_H
#define CODE_RV32_INSTRUCTION_H

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

    void extract_value_and_print(bitset32 mask) {
        bitset32 result;
        for (auto &[pos1, pos2]: bit_positions) {
            result.setBit(pos2, mask[pos1]);
        }
        if (arg_type == reg) {
            printf("%s", registers_names[result.get_bits()].c_str());
        } else {
            int32_t res = result.get_bits();
            if (result[result.get_size() - 1]) {
                res -= (1 << (result.get_size()));
            }
            printf("%d", res);
        }
    }
};

struct RV32_instrucion {
    string name;
    bitset32 equal_mask;
    bitset32 and_mask;
    OutFormat out_format;

    vector<Argument> arguments;

    RV32_instrucion(string name, const Check checks, const vector<Argument> arguments, OutFormat out_format = no_offest) : name(name), arguments(arguments), out_format(out_format) {
        for (int i = 0; i < checks.inds.size(); ++i) {
            equal_mask.setBit(checks.inds[i], checks.vals[i]);
            and_mask.setBit(checks.inds[i], true);
        }
    }

    bool accept(bitset32 msk) {
        return (msk & and_mask) == equal_mask;
    }

    void extract_args(bitset32 row) {
        bool isFirst = true;
        int it = 0;

        for (auto &argument: arguments) {
            if (it == arguments.size() - 1 && out_format == with_offset) printf("(");
            else if (!isFirst) printf(", ");
            argument.extract_value_and_print(row);
            isFirst &= false;
            ++it;
        }

        if (out_format == with_offset) printf(")");
    }
};

vector<int> checks[3] = {
        {0, 1, 2, 3, 4, 5, 6}, //type-u, type-uj
        {0, 1, 2, 3, 4, 5, 6, 12, 13, 14}, //type-i, type-sb, type-s
        {0, 1, 2, 3, 4, 5, 6, 12, 13, 14, 25, 26, 27, 28, 29, 30, 31} //type-r
};

vector<Argument> simm = {Argument({{12, 12},
                                   {13, 13},
                                   {14, 14},
                                   {15, 15},
                                   {16, 16},
                                   {17, 17},
                                   {18, 18},
                                   {19, 19},
                                   {20, 20},
                                   {21, 21},
                                   {22, 22},
                                   {23, 23},
                                   {24, 24},
                                   {25, 25},
                                   {26, 26},
                                   {27, 27},
                                   {28, 28},
                                   {29, 29},
                                   {30, 30},
                                   {31, 31}}, other), //simm[31:12]
                         Argument({{12, 12},
                                   {13, 13},
                                   {14, 14},
                                   {15, 15},
                                   {16, 16},
                                   {17, 17},
                                   {18, 18},
                                   {19, 19},
                                   {20, 11},
                                   {21, 1},
                                   {22, 2},
                                   {23, 3},
                                   {24, 4},
                                   {25, 5},
                                   {26, 6},
                                   {27, 7},
                                   {28, 8},
                                   {29, 9},
                                   {30, 10},
                                   {31, 20}}, other), //simm[20|10:1|11|19:12]
                         Argument({{7,  11},
                                   {8,  1},
                                   {9,  2},
                                   {10, 3},
                                   {11, 4},
                                   {25, 5},
                                   {26, 6},
                                   {27, 7},
                                   {28, 8},
                                   {29, 9},
                                   {30, 10},
                                   {31, 12}}, other), //simm[4:1|11] simm[12|10:5]
                         Argument({{20, 0},
                                   {21, 1},
                                   {22, 2},
                                   {23, 3},
                                   {24, 4},
                                   {25, 5},
                                   {26, 6},
                                   {27, 7},
                                   {28, 8},
                                   {29, 9},
                                   {30, 10},
                                   {31, 11}}, other), //simm[11:0]
                         Argument({{7,  0},
                                   {8,  1},
                                   {9,  2},
                                   {10, 3},
                                   {11, 4},
                                   {25, 5},
                                   {26, 6},
                                   {27, 7},
                                   {28, 8},
                                   {29, 9},
                                   {30, 10},
                                   {31, 11}}, other)}; //simm[4:0] simm[11:5]
Argument rs1 = {{{15, 0}, {16, 1}, {17, 2}, {18, 3}, {19, 4}}, reg};
Argument rs2 = {{{20, 0}, {21, 1}, {22, 2}, {23, 3}, {24, 4}}, reg};
Argument rd = {{{7, 0}, {8, 1}, {9, 2}, {10, 3}, {11, 4}}, reg};
Argument shamt = {{{20, 0}, {21, 1}, {22, 2}, {23, 3}, {24, 4}}, reg};

vector<RV32_instrucion> instrucions = {
        RV32_instrucion("lui", {checks[0], {1, 1, 1, 0, 1, 1, 0}}, {rd, simm[0]}),
        RV32_instrucion{"auipc", {checks[0], {1, 1, 1, 0, 1, 0, 0}}, {rd, simm[0]}},
        RV32_instrucion{"jal", {checks[0], {1, 1, 1, 1, 0, 1, 1}}, {rd, simm[1]}},
        RV32_instrucion{"jalr", {checks[1], {1, 1, 1, 0, 0, 1, 1, 0, 0, 0}}, {rd, rs1, simm[3]}},
        RV32_instrucion{"beq", {checks[1], {1, 1, 0, 0, 0, 1, 1, 0, 0, 0}}, {rs1, rs2, simm[2]}},
        RV32_instrucion{"bne", {checks[1], {1, 1, 0, 0, 0, 1, 1, 1, 0, 0}}, {rs1, rs2, simm[2]}},
        RV32_instrucion{"blt", {checks[1], {1, 1, 0, 0, 0, 1, 1, 0, 0, 1}}, {rs1, rs2, simm[2]}},
        RV32_instrucion{"bge", {checks[1], {1, 1, 0, 0, 0, 1, 1, 1, 0, 1}}, {rs1, rs2, simm[2]}},
        RV32_instrucion{"bltu", {checks[1], {1, 1, 0, 0, 0, 1, 1, 0, 1, 1}}, {rs1, rs2, simm[2]}},
        RV32_instrucion{"bgeu", {checks[1], {1, 1, 0, 0, 0, 1, 1, 1, 1, 1}}, {rs1, rs2, simm[2]}},

        RV32_instrucion{"lb", {checks[1], {1, 1, 0, 0, 0, 0, 0, 0, 0, 0}}, {rd, simm[3], rs1}, with_offset},
        RV32_instrucion{"lh", {checks[1], {1, 1, 0, 0, 0, 0, 0, 1, 0, 0}}, {rd, simm[3], rs1}, with_offset},
        RV32_instrucion{"lw", {checks[1], {1, 1, 0, 0, 0, 0, 0, 0, 1, 0}}, {rd, simm[3], rs1}, with_offset},
        RV32_instrucion{"lbu", {checks[1], {1, 1, 0, 0, 0, 0, 0, 0, 0, 1}}, {rd, simm[3], rs1}, with_offset},
        RV32_instrucion{"lhu", {checks[1], {1, 1, 0, 0, 0, 0, 0, 1, 0, 1}}, {rd, simm[3], rs1}, with_offset},
        RV32_instrucion{"sb", {checks[1], {1, 1, 0, 0, 0, 1, 0, 0, 0, 0}}, {rs2, simm[4], rs1}, with_offset},
        RV32_instrucion{"sh", {checks[1], {1, 1, 0, 0, 0, 1, 0, 1, 0, 0}}, {rs2, simm[4], rs1}, with_offset},
        RV32_instrucion{"sw", {checks[1], {1, 1, 0, 0, 0, 1, 0, 0, 1, 0}}, {rs2, simm[4], rs1}, with_offset},

        RV32_instrucion{"addi", {checks[1], {1, 1, 0, 0, 1, 0, 0, 0, 0, 0}}, {rd, rs1, simm[3]}},
        RV32_instrucion{"slti", {checks[1], {1, 1, 0, 0, 1, 0, 0, 0, 1, 0}}, {rd, rs1, simm[3]}},
        RV32_instrucion{"sltiu", {checks[1], {1, 1, 0, 0, 1, 0, 0, 1, 1, 0}}, {rd, rs1, simm[3]}},
        RV32_instrucion{"xori", {checks[1], {1, 1, 0, 0, 1, 0, 0, 0, 0, 1}}, {rd, rs1, simm[3]}},
        RV32_instrucion{"ori", {checks[1], {1, 1, 0, 0, 1, 0, 0, 0, 1, 1}}, {rd, rs1, simm[3]}},
        RV32_instrucion{"andi", {checks[1], {1, 1, 0, 0, 1, 0, 0, 1, 1, 1}}, {rd, rs1, simm[3]}},
        RV32_instrucion{"slli", {checks[2], {1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, shamt}},
        RV32_instrucion{"srli", {checks[2], {1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, shamt}},
        RV32_instrucion{"srai", {checks[2], {1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0}}, {rd, rs1, shamt}},
        RV32_instrucion{"add", {checks[2], {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"sub", {checks[2], {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"sll", {checks[2], {1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"slt", {checks[2], {1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"sltu", {checks[2], {1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"xor", {checks[2], {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"srl", {checks[2], {1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"sra", {checks[2], {1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"or", {checks[2], {1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"and", {checks[2], {1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"mul", {checks[2], {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"mulh", {checks[2], {1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"mulhsu", {checks[2], {1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"mulhu", {checks[2], {1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"div", {checks[2], {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"divu", {checks[2], {1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"rem", {checks[2], {1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}},
        RV32_instrucion{"remu", {checks[2], {1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}}, {rd, rs1, rs2}}
};

#endif //CODE_RV32_INSTRUCTION_H

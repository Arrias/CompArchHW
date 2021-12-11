#include <iostream>
#include "parser.h"
#include "stream.h"
#include "RV32_Instruction.h"
#include <cstdio>

const int32_t half = 1 << 4;

void print_symtab(const std::vector<Parser::symbol> &symbols, FILE *out) {
    fprintf(out, "Symbol Value              Size Type     Bind     Vis       Index Name\n");
    for (auto &symbol: symbols) {
        fprintf(out, "[%4i] 0x%-15lx %5i %-8s %-8s %-8s %6s %s\n", symbol.symbol_num,
                symbol.symbol_value,
                symbol.symbol_size,
                symbol.symbol_type.c_str(),
                symbol.symbol_bind.c_str(),
                symbol.symbol_visibility.c_str(),
                symbol.symbol_index.c_str(),
                symbol.symbol_name.c_str());
    }
}

int main(int argc, char *argv[]) {
    string inFile = argv[1];
    string outFile = argv[2];

    Parser parser(inFile);
    FILE *out = fopen(outFile.c_str(), "w");

    auto sections = parser.get_sections();
    auto text_section = parser.find_section_by_name(sections, ".text");
    auto symtable_section = parser.find_section_by_name(sections, ".symtab");

    vector<uint8_t> buffer(text_section.section_size);
    for (int i = 0; i < buffer.size(); ++i) {
        buffer[i] = *(parser.memory_pointer + i + text_section.section_offset);
    }

    Stream stream(buffer.begin(), buffer.end());
    auto symtab = parser.get_symbols();
    int32_t address = ((Elf32_Ehdr *) (parser.memory_pointer))->e_entry;

    auto find_mark = [&](uint32_t address) {
        for (auto it = symtab.rbegin(); it != symtab.rend(); ++it) {
            if (it->symbol_value == address) {
                return make_pair(true, it->symbol_name);
            }
        }
        return make_pair(false, string(""));
    };

    while (stream.exist()) {
        auto row = stream.takeN(half);
        auto[fl, mrk] = find_mark(address);

        if (fl && mrk.empty()) {
            sprintf(const_cast<char *>(mrk.c_str()), "LOC_%05x", address);
        }

        if (row[0] && row[1]) {
            fprintf(out, "%08x", address);
            if (fl) {
                fprintf(out, "%10s", mrk.c_str());
            } else {
                fprintf(out, "%10s", "");
            }
            fprintf(out, "   ");
            row.addBits(stream.takeN(half));
            for (auto &instruction: instrucions) {
                if (instruction.accept(row)) {
                    fprintf(out, "%s", instruction.name.c_str());
                    instruction.extract_args(row, out);
                }
            }
        } else { // укороченные команды
            fprintf(out, "unknown command");
        }

        address += row.get_size() / sizeof(uint64_t);
        fprintf(out, "\n");
    }

    fprintf(out, "\n");
    print_symtab(symtab, out);
    return 0;
}

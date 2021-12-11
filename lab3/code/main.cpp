#include <iostream>
#include "parser.h"
#include "stream.h"
#include "RV32_Instruction.h"

const int32_t half = 1 << 4;

void print_symtab(const std::vector<Parser::symbol> &symbols) {
    printf("Symbol Value              Size Type     Bind     Vis       Index Name\n");
    for (auto &symbol: symbols) {
        printf("[%4i] 0x%-15lx %5i %-8s %-8s %-8s %6s %s\n", symbol.symbol_num,
               symbol.symbol_value,
               symbol.symbol_size,
               symbol.symbol_type.c_str(),
               symbol.symbol_bind.c_str(),
               symbol.symbol_visibility.c_str(),
               symbol.symbol_index.c_str(),
               symbol.symbol_name.c_str());
    }
}

int main() {
    string v = "../samples/test_elf.o";
    Parser parser(v);

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

    while (stream.exist()) {
        auto row = stream.takeN(half);

        if (row[0] && row[1]) { // обычные команды
            printf("%08x          ", address);

            row.addBits(stream.takeN(half));

            for (auto &instruction: instrucions) {
                if (instruction.accept(row)) {
                    printf("%s", instruction.name.c_str());
                    instruction.extract_args(row);
                }
            }
            printf("\n");
        } else { // укороченные команды
            printf("unknown command\n");
        }

        address += row.get_size() / sizeof(uint64_t);
    }

    printf("\n");
    print_symtab(symtab);
    return 0;
}


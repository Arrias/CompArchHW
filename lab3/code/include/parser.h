#ifndef CODE_PARSER_H
#define CODE_PARSER_H

#include <cinttypes>
#include <string>
#include <vector>
#include "elf_headers.h"

using std::vector;
using std::string;

struct Parser {
    uint8_t *memory_pointer;

    struct section {
        int section_index = 0;
        intptr_t section_offset, section_addr;
        string section_name;
        string section_type;
        int section_size;
        int section_ent_size;
        int section_addr_align;
    };

    struct symbol {
        string symbol_index;
        intptr_t symbol_value;
        int symbol_num = 0;
        int symbol_size = 0;
        string symbol_type;
        string symbol_bind;
        string symbol_visibility;
        string symbol_name;
        string symbol_section;
    };

    section &find_section_by_name(vector<Parser::section> &sections, const string &name);

    Parser(string path);

    string get_symbol_visibility(uint8_t sym_vis);

    string get_section_type(int type);

    std::vector<section> get_sections();

    std::string get_symbol_type(uint8_t sym_type);

    string get_symbol_bind(uint8_t sym_bind);

    string get_symbol_index(uint16_t sym_idx);

    std::vector<symbol> get_symbols();
};

#endif //CODE_PARSER_H
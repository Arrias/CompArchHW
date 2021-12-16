#include "parser.h"
#include <iostream>

Parser::Parser(string path) {
    memory_pointer = fopen(path.c_str(), "rb");
    if (memory_pointer == nullptr) {
        std::cout << "File open error\n";
        exit(0);
    }
    fread(&ehdr, sizeof(ehdr), 1, memory_pointer);
    fseek(memory_pointer, ehdr.e_shoff, SEEK_SET);
    fread(&shdr, sizeof(shdr), 1, memory_pointer);
}

vector<string> sections_types = {"SHT_NULL", "SHT_PROGBITS", "SHT_SYMTAB", "SHT_STRTAB", "SHT_RELA", "SHT_HASH", "SHT_DYNAMIC",
                                 "SHT_NOTE", "SHT_NOBITS", "SHT_REL", "SHT_DYNSYM"};

string Parser::get_section_type(int type) {
    if (type < 0 || type >= sections_types.size()) return "UNKNOWN";
    return sections_types[type];
}

#define MAX_SIZE 1500
char buf[MAX_SIZE];

vector<Parser::section> Parser::get_sections() {
    int shnum = ehdr.e_shnum;
    fseek(memory_pointer, ehdr.e_shoff + sizeof(Elf32_Shdr) * ehdr.e_shstrndx, SEEK_SET);
    Elf32_Shdr sh_strtab;
    fread(&sh_strtab, sizeof(Elf32_Shdr), 1, memory_pointer);
    fseek(memory_pointer, sh_strtab.sh_offset, SEEK_SET);

    vector<section> ret;

    for (int i = 0; i < shnum; ++i) {
        fseek(memory_pointer, ehdr.e_shoff + sizeof(Elf32_Shdr) * i, SEEK_SET);
        Elf32_Shdr cur;
        fread(&cur, sizeof(Elf32_Shdr), 1, memory_pointer);

        section section;

        fseek(memory_pointer, sh_strtab.sh_offset + cur.sh_name, SEEK_SET);
        fscanf(memory_pointer, "%s", buf);

        section.section_name = std::string(buf);
        section.section_type = get_section_type(cur.sh_type);
        section.section_addr = cur.sh_addr;
        section.section_offset = cur.sh_offset;
        section.section_size = cur.sh_size;
        section.section_ent_size = cur.sh_entsize;
        section.section_addr_align = cur.sh_addralign;
        ret.push_back(section);
    }

    return ret;
}

vector<string> symbol_types = {"NOTYPE", "OBJECT", "FUNC", "SECTION", "FILE", "TLS", "NUM", "LOOS", "HIOS"};

std::string Parser::get_symbol_type(uint8_t sym_type) {
    auto num = ELF32_ST_TYPE(sym_type);
    if (num >= symbol_types.size()) return "UNKNOWN";
    return symbol_types[num];
}

vector<string> symbol_binds = {"LOCAL", "GLOBAL", "WEAK", "NUM", "UNIQUE", "HIOS", "LOPROC"};

string Parser::get_symbol_bind(uint8_t sym_bind) {
    auto num = ELF32_ST_BIND(sym_bind);
    if (num >= symbol_binds.size()) return "UNKNOWN";
    return symbol_binds[num];
}

string Parser::get_symbol_index(uint16_t sym_idx) {
    if (sym_idx == SHN_ABS) return "ABS";
    if (sym_idx == SHN_COMMON) return "COMMON";
    if (sym_idx == SHN_UNDEF) return "UNDEF";
    if (sym_idx == SHN_XINDEX) return "XINDEX";
    return std::to_string(sym_idx);
}

uint32_t get_shift(vector<Parser::section> &sections, string section_type, string section_name) {
    for (auto &sec: sections) {
        if (sec.section_type == section_type && sec.section_name == section_name) {
            return sec.section_offset;
        }
    }
    return 0;
}

vector<Parser::symbol> Parser::get_symbols() {
    std::vector<section> secs = get_sections();

    auto strtab_shift = get_shift(secs, "SHT_STRTAB", ".strtab");
    auto dynstr_shift = get_shift(secs, "SHT_STRTAB", ".dynstr");

    vector<symbol> ret;

    for (auto &sec: secs) {
        if ((sec.section_type != "SHT_SYMTAB") && (sec.section_type != "SHT_DYNSYM"))
            continue;

        auto sz = sec.section_size / sizeof(Elf32_Sym);

        for (int i = 0; i < sz; ++i) {
            Parser::symbol cur_symbol;
            cur_symbol.symbol_num = i;

            fseek(memory_pointer, sec.section_offset + sizeof(Elf32_Sym) * i, SEEK_SET);
            Elf32_Sym sym;
            fread(&sym, sizeof(Elf32_Sym), 1, memory_pointer);

            cur_symbol.symbol_value = sym.st_value;
            cur_symbol.symbol_size = sym.st_size;
            cur_symbol.symbol_type = get_symbol_type(sym.st_info);
            cur_symbol.symbol_bind = get_symbol_bind(sym.st_info);
            cur_symbol.symbol_visibility = get_symbol_visibility(sym.st_other);
            cur_symbol.symbol_index = get_symbol_index(sym.st_shndx);
            cur_symbol.symbol_section = sec.section_name;

            if (sec.section_type == "SHT_SYMTAB") {
                fseek(memory_pointer, strtab_shift + sym.st_name, SEEK_SET);
                fscanf(memory_pointer, "%s", buf);
            }
            if (sec.section_type == "SHT_DYNSYM") {
                fseek(memory_pointer, dynstr_shift + sym.st_name, SEEK_SET);
                fscanf(memory_pointer, "%s", buf);
            }
            cur_symbol.symbol_name = std::string(buf);
            ret.push_back(cur_symbol);
        }
    }
    return ret;
}

vector<string> symbol_visibilities = {"DEFAULT", "INTERNAL", "HIDDEN", "PROTECTED"};

string Parser::get_symbol_visibility(uint8_t sym_vis) {
    auto num = ELF32_ST_VISIBILITY(sym_vis);
    if (num >= symbol_visibilities.size()) return "UNKNOWN";
    return symbol_visibilities[num];
}

Parser::section &Parser::find_section_by_name(vector<Parser::section> &sections, const string &name) {
    for (auto &s: sections) {
        if (s.section_name == name) {
            return s;
        }
    }
}

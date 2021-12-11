#include "parser.h"

Parser::Parser(string path) {
    int fd, i;
    struct stat st;
    fd = open(path.c_str(), O_RDONLY);
    fstat(fd, &st);
    memory_pointer = (uint8_t *) (mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
}

vector<string> sections_types = {"SHT_NULL", "SHT_PROGBITS", "SHT_SYMTAB", "SHT_STRTAB", "SHT_RELA", "SHT_HASH", "SHT_DYNAMIC",
                                 "SHT_NOTE", "SHT_NOBITS", "SHT_REL", "SHT_DYNSYM"};

string Parser::get_section_type(int type) {
    if (type < 0 || type >= sections_types.size()) return "UNKNOWN";
    return sections_types[type];
}

vector<Parser::section> Parser::get_sections() {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *) memory_pointer;
    Elf32_Shdr *shdr = (Elf32_Shdr *) (memory_pointer + ehdr->e_shoff);
    int shnum = ehdr->e_shnum;

    Elf32_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
    const char *const sh_strtab_p = (char *) memory_pointer + sh_strtab->sh_offset;

    std::vector<Parser::section> sections;
    for (int i = 0; i < shnum; ++i) {
        Parser::section section;
        section.section_index = i;
        section.section_name = std::string(sh_strtab_p + shdr[i].sh_name);
        section.section_type = get_section_type(shdr[i].sh_type);
        section.section_addr = shdr[i].sh_addr;
        section.section_offset = shdr[i].sh_offset;
        section.section_size = shdr[i].sh_size;
        section.section_ent_size = shdr[i].sh_entsize;
        section.section_addr_align = shdr[i].sh_addralign;
        sections.push_back(section);
    }
    return sections;
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

    char *sh_strtab_p = (char *) memory_pointer + get_shift(secs, "SHT_STRTAB", ".strtab");
    char *sh_dynstr_p = (char *) memory_pointer + get_shift(secs, "SHT_STRTAB", ".dynstr");

    vector<symbol> ret;

    for (auto &sec: secs) {
        if ((sec.section_type != "SHT_SYMTAB") && (sec.section_type != "SHT_DYNSYM"))
            continue;

        auto sz = sec.section_size / sizeof(Elf32_Sym);
        auto syms_data = (Elf32_Sym *) (memory_pointer + sec.section_offset);

        for (int i = 0; i < sz; ++i) {
            Parser::symbol cur_symbol;
            cur_symbol.symbol_num = i;
            cur_symbol.symbol_value = syms_data[i].st_value;
            cur_symbol.symbol_size = syms_data[i].st_size;
            cur_symbol.symbol_type = get_symbol_type(syms_data[i].st_info);
            cur_symbol.symbol_bind = get_symbol_bind(syms_data[i].st_info);
            cur_symbol.symbol_visibility = get_symbol_visibility(syms_data[i].st_other);
            cur_symbol.symbol_index = get_symbol_index(syms_data[i].st_shndx);
            cur_symbol.symbol_section = sec.section_name;

            if (sec.section_type == "SHT_SYMTAB")
                cur_symbol.symbol_name = std::string(sh_strtab_p + syms_data[i].st_name);

            if (sec.section_type == "SHT_DYNSYM")
                cur_symbol.symbol_name = std::string(sh_dynstr_p + syms_data[i].st_name);

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

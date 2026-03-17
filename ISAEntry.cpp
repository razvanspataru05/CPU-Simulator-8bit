#include "ISAEntry.h"

struct ISAEntry
{
    const char* mnemonic;
    uint8_t opcode;
    uint8_t size;
    const char* description;
};

static const ISAEntry isaTable[] = {
    {"NOP", 0x00, 1, "No operation"},

    /* Load / Store instructions */
    {"LDA_IM", 0x01, 2, "Loads immediate value into register A"},
    {"LDA_DIR", 0x02, 3, "Loads value at immediate memory address into register A"},
    {"LDA_REG", 0x03, 2, "Loads value from another register into register A"},
    {"STA_DIR", 0x04, 3, "Writes value of register A at memory address specified"},
    {"LDB_IM", 0x05, 2, "Loads immediate value into register B" },
    {"LDB_DIR", 0x06, 3, "Loads value at immediate memory address into register B"},
    {"LDB_REG", 0x07, 2, "Loads value from another register into register B"},
    {"STB_DIR", 0x08, 3, "Writes value of register B at memory address specified"},
    {"LDC_IM", 0x09, 2, "Loads immediate value into register C" },
    {"LDC_DIR", 0x0A, 3, "Loads value at immediate memory address into register C"},
    {"LDC_REG", 0x0B, 2, "Loads value from another register into register C"},
    {"STC_DIR", 0x0C, 3, "Writes value of register C at memory address specified"},
    {"LDD_IM", 0x0D, 2, "Loads immediate value into register D" },
    {"LDD_DIR", 0x0E, 3, "Loads value at immediate memory address into register D"},
    {"LDD_REG", 0x0F, 2, "Loads value from another register into register D"},
    {"STD_DIR", 0x10, 3, "Writes value of register D at memory address specified"},

    /* Logical operations */
    {"ADD_IM", 0x20, 2, "Adds immediate value to register A. Updates flags"},
    {"ADD_REG", 0x21, 2, "Adds another register value to register A. Updates flags"},
    {"ADC_IM"}
};
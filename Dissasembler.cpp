#include "Dissasembler.h"

Dissasembler::Dissasembler()
{
	m_instructions[0x00] = { "NOP", 1u };
	m_instructions[0x01] = { "LDA_IM", 2u };
	m_instructions[0x02] = { "LDA_DIR", 2u };
	m_instructions[0x03] = { "STA_DIR", 2u };
	m_instructions[0x04] = {"ADD_IM", 2u};
	m_instructions[0x05] = { "JMP", 2u };
	m_instructions[0x06] = { "JZ", 2u };
	m_instructions[0xFF] = { "HLT", 1u };
}

const InstructionDef& Dissasembler::GetInstructionDef(uint8_t index) const
{
	return m_instructions[index];
}

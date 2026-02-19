#pragma once
#include "InstructionDef.h"
#include <array>

class Dissasembler
{
public:
	Dissasembler();

	[[nodiscard]] const InstructionDef& GetInstructionDef(uint8_t index) const;

private:
	std::array<InstructionDef, 256> m_instructions;
};


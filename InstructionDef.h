#pragma once
#include <cstdint>
#include <string>

struct InstructionDef
{
	std::string mnemonic;
	uint8_t size;
};
#pragma once
#include "MemoryUnit.h"

class CPU
{
public:
	CPU(MemoryUnit& m_memoryUnit);

	void Step();
	void Reset();

	[[nodiscard]] uint8_t GetA() const;
	[[nodiscard]] uint8_t GetPC() const;
	[[nodiscard]] uint8_t GetIR() const;
	[[nodiscard]] bool GetZeroFlag() const;
	[[nodiscard]] bool GetHaltFlag() const;

private:

	uint8_t m_A; // Accumulator registry
	uint8_t m_PC; // Program Counter registry
	uint8_t m_IR; // Instruction registry
	bool m_ZeroFlag;
	bool m_HaltFlag;
	MemoryUnit& m_memoryUnit;
};


#pragma once
#include <array>
#include <vector>
#include <fstream>

class MemoryUnit
{
public:
	MemoryUnit();

	[[nodiscard]] const std::array<uint8_t, 256>& GetMemory() const;
	[[nodiscard]] uint8_t Read(uint8_t address) const;
	void Write(uint8_t address, uint8_t value);
	void Reset();
	void Load(uint8_t startAddress, const std::vector<uint8_t>& values);
	void ParseValues();

private:
	std::array<uint8_t, 256> m_memory;
};


#pragma once
#include <array>
#include <vector>
#include <fstream>

class MemoryUnit
{
public:
	MemoryUnit();

	[[nodiscard]] const std::array<uint8_t, 65536>& GetMemory() const noexcept;
	[[nodiscard]] uint8_t Read(uint16_t address) const noexcept;
	void Write(uint16_t address, uint8_t value) noexcept;
	void Reset(const std::array<uint8_t, 65536>& initialMemory) noexcept;
	void Load(uint16_t startAddress, const std::vector<uint8_t>& values);
	void ParseValues();

private:
	std::array<uint8_t, 65536> m_memory;
};


#include "CPU.h"

CPU::CPU(MemoryUnit& memoryUnit) :
	m_memoryUnit{ memoryUnit }
{
	m_A = 0u;
	m_PC = 0u;
	m_IR = 0u;
	m_ZeroFlag = false;
	m_HaltFlag = false;
}

void CPU::Step()
{
	if (m_HaltFlag) return;

	m_IR = m_memoryUnit.Read(m_PC);
	m_PC++;

	switch (m_IR)
	{
	case 0x00:
		return;
	case 0x01:
		m_A = m_memoryUnit.Read(m_PC);
		m_PC++;
		return;
	case 0x02:
	{
		auto operand = m_memoryUnit.Read(m_PC);
		m_A = m_memoryUnit.Read(operand);
		m_PC++;
		return;
	}	
	case 0x03:
	{
		auto operand = m_memoryUnit.Read(m_PC);
		m_memoryUnit.Write(operand, m_A);
		m_PC++;
		return;
	}
	case 0x04:
		m_A = m_A + m_memoryUnit.Read(m_PC);
		m_PC++;
		m_ZeroFlag = m_A == 0;
		return;
	case 0x05:
		m_PC = m_memoryUnit.Read(m_PC);
		return;
	case 0x06:
		if (m_ZeroFlag) m_PC = m_memoryUnit.Read(m_PC);
		else m_PC++;
		return;
	case 0xFF:
		m_HaltFlag = true;
		return;
	}
}

uint8_t CPU::GetA() const
{
	return m_A;
}

uint8_t CPU::GetPC() const
{
	return m_PC;
}

uint8_t CPU::GetIR() const
{
	return m_IR;
}

bool CPU::GetZeroFlag() const
{
	return m_ZeroFlag;
}

bool CPU::GetHaltFlag() const
{
	return m_HaltFlag;
}

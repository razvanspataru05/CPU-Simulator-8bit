#include "EditorUI.h"

uint8_t helperPage = 1u;
uint16_t currentPage{};
constexpr uint16_t PAGE_SIZE = 256u;
bool visibility = false;

static const char* FlagToStr(bool value)
{
	return value ? "True" : "False";
}

namespace EditorUI
{
	void DrawCpuState(const CPU& cpu)
	{
		ImGui::Begin("CPU State");
		ImGui::Text("Program Counter: 0x%04X", cpu.GetPC());
		ImGui::Text("Stack Pointer: 0x%02X", cpu.GetSP());
		ImGui::Text("Instruction Register: 0x%02X", cpu.GetIR());

		ImGui::Text("\nRegistry");
		ImGui::Text("A: %d", cpu.GetA());
		ImGui::Text("B: %d", cpu.GetB());
		ImGui::Text("C: %d", cpu.GetC());
		ImGui::Text("D: %d", cpu.GetD());

		ImGui::Text("\nFlags");
		ImGui::Text("Zero Flag: %s", FlagToStr(cpu.GetZeroFlag()));
		ImGui::Text("Carry Flag: %s", FlagToStr(cpu.GetCarryFlag()));
		ImGui::Text("Negative Flag: %s", FlagToStr(cpu.GetNegativeFlag()));
		ImGui::Text("Overflow Flag: %s", FlagToStr(cpu.GetOverflowFlag()));
		ImGui::Text("Halt Flag: %s", FlagToStr(cpu.GetHaltFlag()));
		ImGui::End();
	}

	void DrawDissasembler(const MemoryUnit& memoryUnit, const Dissasembler& dissasembler, const CPU& cpu)
	{
		ImGui::Begin("Disassembler");
		for (size_t index = 0; index < memoryUnit.GetMemory().size(); ++index)
		{
			uint8_t opcode = memoryUnit.Read(static_cast<uint16_t>(index));
			const InstructionDef instruction = dissasembler.GetInstructionDef(opcode);

			if (index == cpu.GetPC())
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
			else
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

			switch (instruction.size)
			{
			case 1:
			{
				ImGui::Text("0x%02x: %s", opcode, instruction.mnemonic.c_str());
				break;
			}
			case 2:
			{
				uint8_t secondByte = memoryUnit.Read(static_cast<uint16_t>(index + 1));
				ImGui::Text("0x%02x: %s %d", opcode, instruction.mnemonic.c_str(), secondByte);
				break;
			}
			case 3:
			{
				uint8_t secondByte = memoryUnit.Read(static_cast<uint16_t>(index + 1));
				uint8_t thirdByte = memoryUnit.Read(static_cast<uint16_t>(index + 2));
				uint16_t address = (secondByte << 8) | thirdByte;
				ImGui::Text("0x%02x: %s 0x%04X", opcode, instruction.mnemonic.c_str(), address);
				break;
			}
			default:
				ImGui::Text("0x%04zx: ???", index);
				break;
			}
			index += instruction.size - 1;
			ImGui::PopStyleColor();
			if (opcode == 0xFF) break;
		}
		ImGui::End();
	}
	void DrawMemoryView(const MemoryUnit& memoryUnit, CPU& cpu, bool& followPC)
	{
		if (followPC)
			currentPage = cpu.GetPC() / PAGE_SIZE;

		std::string memoryView = "Memory View (page " + std::to_string(currentPage) + "/" + std::to_string(PAGE_SIZE - 1) + ")";
		ImGui::Begin(memoryView.c_str());
		if (ImGui::Button("\t\t\tPrev\t\t\t"))
			currentPage = currentPage - 1 < 0 ? 255 : currentPage - 1;

		ImGui::SameLine();
		ImGui::Text("                                                                              ");
		ImGui::SameLine();
		if (ImGui::Button("\t\t\tNext\t\t\t"))
			currentPage = (currentPage + 1) % 256;

		const size_t startAddress = currentPage * PAGE_SIZE;
		const size_t endAddress = startAddress + PAGE_SIZE;
		if (ImGui::BeginTable("MemoryTable", 16, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			for (size_t index = startAddress; index < endAddress; ++index)
			{
				ImGui::TableNextColumn();
				if (index == cpu.GetPC() && !cpu.GetHaltFlag())
				{
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 0, 255, 255));
				}

				if (cpu.IsReadingInstruction() && index == cpu.ComputeAddress(cpu.GetPC() + 1))
				{
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 255, 0, 255));
				}
				else if (cpu.IsWritingInstruction() &&
					index == cpu.ComputeAddress(cpu.GetPC() + 1))
				{
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 0, 0, 255));
				}

				ImGui::Text("0x%02X", memoryUnit.Read(static_cast<uint16_t>(index)));
			}
			ImGui::EndTable();
		}
		ImGui::End();
	}
	void DrawMenu(MemoryUnit& memoryUnit, const std::array<uint8_t, 65536>& initialMemory,
		bool& executeAuto, bool& followPC, CPU& cpu)
	{
		std::string execute = "Auto (";
		execute += executeAuto ? "ON)" : "OFF)";
		std::string follow = "Follow PC (";
		follow += followPC ? "ON)" : "OFF)";

		ImGui::Begin("Menu");
		if (ImGui::Button("\t\tNext Step\t\t"))
		{
			cpu.Step();
		}
		ImGui::SameLine();

		if (ImGui::Button("\t\tReset\t\t"))
		{
			memoryUnit.Reset(initialMemory);
			cpu.Reset();
			executeAuto = false;
			followPC = true;
		}

		if (ImGui::Button(execute.c_str()))
		{
			executeAuto = !executeAuto;
		}
		ImGui::SameLine();

		if (ImGui::Button(follow.c_str()))
		{
			followPC = !followPC;
		}
		ImGui::End();

		ImGui::Begin("Help");
		if (ImGui::Button("Help"))
		{
			visibility = true;
		}
		ImGui::End();
	}
	void DrawSpeedSlider(float& speed)
	{
		ImGui::Begin("Speed Slider");
		ImGui::SliderFloat("Speed (seconds)", &speed, 0.1f, 2.0f);
		ImGui::End();
	}
	void DrawHelpMenu()
	{
		if (visibility)
		{
			ImGui::Begin("Help Menu");
			std::string pageText = "Page " + std::to_string(helperPage) + "/7";
			float windowWidth = ImGui::GetWindowSize().x;
			float textWidth = ImGui::CalcTextSize(pageText.c_str()).x;
			float navButtonWidth = ImGui::CalcTextSize(">").x + ImGui::GetStyle().FramePadding.x * 2;
			float buttonWidth = ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x * 2;

			float centerX = (windowWidth - textWidth) * 0.5f;
			ImGui::SetCursorPosX(centerX);
			ImGui::Text(pageText.c_str());
			ImGui::SameLine();

			ImGui::SetCursorPosX(windowWidth - buttonWidth - ImGui::GetStyle().WindowPadding.x);
			if (ImGui::Button("X"))
			{
				visibility = false;
			}

			ImGui::SetCursorPosY(windowWidth * 0.055f - ImGui::GetStyle().WindowPadding.y);
			if (ImGui::Button("<") && helperPage > 1)
				helperPage--;

			ImGui::SameLine();
			ImGui::SetCursorPosX(windowWidth - navButtonWidth - ImGui::GetStyle().WindowPadding.x);
			ImGui::SetCursorPosY(windowWidth * 0.055f - ImGui::GetStyle().WindowPadding.y);
			if (ImGui::Button(">") && helperPage < 7)
				helperPage++;

			switch (helperPage)
			{
			case 1u:
			{
				const char* title = "Glossary";
				float titleWidth = ImGui::CalcTextSize(title).x;
				ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
				ImGui::SetCursorPosY(windowWidth * 0.05f);
				ImGui::Text(title);
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::BeginTable("glossary_table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg))
				{
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 120.0f);
					ImGui::TableSetupColumn("Definition", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableHeadersRow();

					static const std::pair<const char*, const char*> glossary[] =
					{
						{"PC", "Program Counter"},
						{"IR", "Instruction Register"},
						{"SP", "Stack Pointer"},
						{"A", "Accumulator Register"},
						{"B", ""},
						{"C", "Counter Register"}
					};

					for (const auto& [mnemonic, definition] : glossary)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), mnemonic);
						ImGui::TableSetColumnIndex(1);
						ImGui::TextWrapped(definition);
					}
					ImGui::EndTable();
				}
				break;
			}

			}
			ImGui::End();
		}
	}
}
#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include "framework.h"
#include <fcntl.h>
#include <sstream>

#include "MemoryUnit.h"
#include "CPU.h"
#include "Dissasembler.h"

static const char* FlagToStr(bool value)
{
    return value ? "True" : "False";
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    MemoryUnit memory;
    memory.ParseValues();
    const std::array<uint8_t, 65536> initialMemory = memory.GetMemory();

    CPU cpu(memory);
    Dissasembler dissasembler;
    const size_t memorySize = memory.GetMemory().size();

    sf::Font font;

    sf::RenderWindow window(sf::VideoMode({1920, 1080}), "CPU Simulator");
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, event.value());
            if (event->is<sf::Event::KeyPressed>())
            {
                const auto keyCode = event->getIf<sf::Event::KeyPressed>();
                if (keyCode->code == sf::Keyboard::Key::Space)
                {
                    cpu.Step();
                }
                else if (keyCode->code == sf::Keyboard::Key::R)
                {
                    memory.Reset(initialMemory);
                    cpu.Reset();
                }
            }

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());    

        window.clear(sf::Color(50, 50, 50));

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
        
        ImGui::Begin("Disassembler");
        for (size_t index = 0; index < memory.GetMemory().size(); ++index)
        {
            uint8_t opcode = memory.Read(static_cast<uint16_t>(index));
            const InstructionDef instruction = dissasembler.GetInstructionDef(opcode);

            if (index==cpu.GetPC())
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
                uint8_t secondByte = memory.Read(static_cast<uint16_t>(index + 1));
                ImGui::Text("0x%02x: %s %d", opcode, instruction.mnemonic.c_str(), secondByte);
                break;
            }
            case 3:
            {
                uint8_t secondByte = memory.Read(static_cast<uint16_t>(index + 1));
                uint8_t thirdByte = memory.Read(static_cast<uint16_t>(index + 2));
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

        ImGui::Begin("Memory View");

        if (ImGui::BeginTable("MemoryTable", 16, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            for (size_t index = 0; index < memory.GetMemory().size(); ++index)
            {
                ImGui::TableNextColumn();

                if (index == cpu.GetPC() && !cpu.GetHaltFlag())
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 0, 255, 255));
                }

                if (cpu.IsReadingInstruction() && index == memory.Read(cpu.GetPC()+1))
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 255, 0, 255));
                }
                else if (cpu.IsWritingInstruction() &&
                    index == cpu.ComputeAddress(cpu.GetPC()+1))
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 0, 0, 255));
                }

                ImGui::Text("0x%02X", memory.Read(static_cast<uint8_t>(index)));
            }
            ImGui::EndTable();
        }
        ImGui::End();

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
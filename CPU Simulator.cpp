#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include "framework.h"
#include <fcntl.h>
#include <sstream>

#include "MemoryUnit.h"
#include "CPU.h"
#include "Dissasembler.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    MemoryUnit memory;
    memory.ParseValues();
    const std::array<uint8_t, 256> initialMemory = memory.GetMemory();

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
        ImGui::Text("Program Counter: 0x%02X", cpu.GetPC());
        ImGui::Text("Instruction Register: 0x%02X", cpu.GetIR());
        ImGui::Text("A (Accumulator): %d", cpu.GetA());
        ImGui::Text("");
        if (cpu.GetZeroFlag())
        {
            ImGui::Text("Zero Flag: True");
        }
        else ImGui::Text("Zero Flag: False");

        if (cpu.GetHaltFlag())
        {
            ImGui::Text("Halt Flag: True");
        }
        else ImGui::Text("Halt Flag: False");
        ImGui::End();
        
        ImGui::Begin("Disassembly");
        for (size_t index = 0; index < memory.GetMemory().size(); ++index)
        {
            uint8_t opcode = memory.Read(static_cast<uint8_t>(index));
            const InstructionDef instruction = dissasembler.GetInstructionDef(opcode);
            if (instruction.size == 2)
            {
                if (index == cpu.GetPC())
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                }
                else 
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

                uint8_t nextByte = memory.Read(static_cast<uint8_t>(index+1));
                ++index;
                ImGui::Text("0X%02x: %s %d", opcode, instruction.mnemonic.c_str(), nextByte);
                ImGui::PopStyleColor();
            }
            else
            {
                if (index == cpu.GetPC())
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                }
                else
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

                ImGui::Text("0X%02x: %s", opcode, instruction.mnemonic.c_str());
                ImGui::PopStyleColor();
            }
            if (opcode == 0xFF) break;
        }
        ImGui::End();

        ImGui::Begin("Memory View");

        if (ImGui::BeginTable("MemoryTable", 16, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            for (size_t index = 0; index < 256; ++index)
            {
                ImGui::TableNextColumn();
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
#include "framework.h"
#include <iostream>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <sstream>
#include <iomanip>

#include "CPU Simulator.h"
#include "MemoryUnit.h"
#include "CPU.h"
#include "Dissasembler.h"
#include "SFML/Graphics.hpp"

constexpr uint8_t CELL_HEIGHT{ 40u };
constexpr uint8_t CELL_WIDTH{ 40u };
constexpr uint8_t Y_START{ 50u };
constexpr uint8_t X_START{ 150u };
constexpr uint8_t PADDING{ 40u };

void static CreateConsole()
{
    AllocConsole();
    FILE* dummy;

    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONIN$", "r", stdin);
    freopen_s(&dummy, "CONOUT$", "w", stderr);
    
    std::ios::sync_with_stdio;
}

void static SetupText(const CPU& cpu, std::stringstream& ss)
{
    ss << "Program Counter: 0x"
        << std::uppercase << std::setfill('0') << std::setw(2) << std::hex
        << static_cast<int>(cpu.GetPC()) << "\n\n";

    ss << "Instruction Register: 0x"
        << std::uppercase << std::setfill('0') << std::setw(2) << std::hex
        << static_cast<int>(cpu.GetIR()) << "\n\n";

    ss << "A (Accumulator) Register: 0x"
        << std::uppercase << std::setfill('0') << std::setw(2) << std::hex
        << static_cast<int>(cpu.GetA());
}

void static SetupAssemblyCode(const Dissasembler& dissasembler, 
    const MemoryUnit& memoryUnit, std::stringstream& ss)
{
    for (size_t index = 0; index < memoryUnit.GetMemory().size(); index++)
    {
        uint8_t opcode = memoryUnit.Read(index);
        const auto instruction = dissasembler.GetInstructionDef(opcode);
        ss << instruction.mnemonic;
        if (instruction.size == 2)
        {
            uint8_t nextByte = memoryUnit.Read(index + 1);
            ss << " " << static_cast<int>(nextByte);
            index++;
        }
        ss << "\n";
        if (opcode == 0xFF) break;
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    CreateConsole();
    std::cout << "Consola\n";
    MemoryUnit memory;
    memory.ParseValues();

    CPU cpu(memory);
    Dissasembler dissasembler;
    const size_t memorySize = memory.GetMemory().size();

    sf::Font font;
    if (!font.openFromFile("Roboto_Mono.ttf"))
    {
        throw std::runtime_error("Roboto_Mono.ttf not found");
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode({1920, 1080}), "CPU Simulator");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
        window.clear(sf::Color::White);

        sf::Text assemblyText(font);
        assemblyText.setCharacterSize(24);
        assemblyText.setFillColor(sf::Color::Black);
        assemblyText.setPosition({1000, 50});

        std::stringstream assemblyStringStream;
        SetupAssemblyCode(dissasembler, memory, assemblyStringStream);
        assemblyText.setString(assemblyStringStream.str());
        window.draw(assemblyText);

        float xCoord = X_START;
        float yCoord = Y_START;
        sf::RectangleShape memoryCell;
        sf::Text text(font);
        sf::Text programCounterText(font);
        programCounterText.setPosition({ 50, 750 });
        programCounterText.setCharacterSize(24);
        programCounterText.setFillColor(sf::Color::Black);

        std::stringstream ss;
        SetupText(cpu, ss);
        programCounterText.setString(ss.str());
        
        window.draw(programCounterText);

        for (size_t index = 0; index < memorySize; index++)
        {
            if (index != 0 && index % 16 == 0)
            {
                xCoord = X_START;
                yCoord += PADDING;
            }
            std::stringstream ss;
            ss << "0x" << std::uppercase << std::setfill('0') << std::setw(2) <<
                std::hex << static_cast<int>(memory.Read(static_cast<uint8_t>(index)));
            text.setCharacterSize(12);
            text.setFillColor(sf::Color::Black);
            text.setOutlineColor(sf::Color::Red);
            text.setOutlineThickness(1.f);
            text.setString(ss.str());
            memoryCell.setSize({ CELL_HEIGHT, CELL_WIDTH });
            memoryCell.setFillColor(sf::Color(128, 128, 128));
            memoryCell.setPosition({ xCoord, yCoord});
            memoryCell.setOutlineColor(sf::Color::Black);
            memoryCell.setOutlineThickness(2.f);
            auto bounds = text.getLocalBounds();
            text.setPosition({
                memoryCell.getPosition().x + memoryCell.getSize().x / 2.f
                - bounds.size.x / 2.f - bounds.position.x,
                memoryCell.getPosition().y + memoryCell.getSize().y / 2.f
                - bounds.size.y / 2.f - bounds.position.y
                });
            window.draw(memoryCell);
            window.draw(text);
            xCoord += PADDING;
        }
        window.display();
    }

    return 0;
}
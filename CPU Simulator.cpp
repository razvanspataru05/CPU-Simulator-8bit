#include "framework.h"
#include <fcntl.h>
#include <sstream>

#include "MemoryUnit.h"
#include "Dissasembler.h"
#include "EditorUI.h"

bool executeAuto = false; 
float speed = 1.0f;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    sf::Clock instructionCycle;
    MemoryUnit memory;
    memory.ParseValues();
    const std::array<uint8_t, 65536> initialMemory = memory.GetMemory();

    CPU cpu(memory);
    Dissasembler dissasembler;

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

        if (executeAuto && instructionCycle.getElapsedTime().asSeconds() >= speed)
        {
            cpu.Step();
            instructionCycle.restart();
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        window.clear(sf::Color(65, 65, 65));

        EditorUI::DrawCpuState(cpu);
        EditorUI::DrawDissasembler(memory, dissasembler, cpu);
        EditorUI::DrawMemoryView(memory, cpu);
        EditorUI::DrawMenu(memory, initialMemory, executeAuto, cpu);
        EditorUI::DrawSpeedSlider(speed);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
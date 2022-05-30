#include <SFML/Graphics.hpp>
#include <iostream>
#include "PacView.hpp"
#include "Menu.hpp"
#include "tinyfiledialogs.h"

using namespace std;

int main(int argc, char* argv[])
{
    cout << "PacViewer v3.0.12 by Owocek" << endl;

    std::string pac_dir;

    if (argc <= 1)
    {
        char const* lFilterPatterns[1] = { "*.pac" };
        const char* path = tinyfd_openFileDialog(
            "Select a .pac file to open",
            "",
            1,
            lFilterPatterns,
            "Patapon Assembly Code files (*.pac)",
            0);

        if (path == NULL)
        {
            cout << "No file specified. Program will close now." << endl;
            return 1;
        }
        else
        {
            pac_dir = path;
        }
    }

    if (argc > 1)
    {
        pac_dir = argv[1];
    }

    string workdir = argv[0];
    workdir = workdir.substr(0,workdir.find_last_of("\\")+1);

    Menu menu(workdir);

    std::unique_ptr<PacView> pac;

    map<int,bool> mouseMap;
    map<int,bool> keyMap;
    sf::Vector2i mousePos;

    sf::RenderWindow window(sf::VideoMode(960, 640), "PacViewer");
    window.setFramerateLimit(60);

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if(event.type == sf::Event::MouseWheelMoved)
            {
                pac->scroll += event.mouseWheel.delta * (-2);

                if(pac->scroll <= 0)
                pac->scroll = 0;
            }
            else if(event.type == sf::Event::MouseButtonPressed)
            {
                mouseMap[event.mouseButton.button] = true;
                mousePos = sf::Vector2i(event.mouseButton.x,event.mouseButton.y);
            }
            else if(event.type == sf::Event::MouseButtonReleased)
            {
                mouseMap[event.mouseButton.button] = false;
            }
            else if(event.type == sf::Event::MouseMoved)
            {
                mousePos = sf::Vector2i(event.mouseMove.x,event.mouseMove.y);
            }
            else if(event.type == sf::Event::KeyPressed)
            {
                keyMap[event.key.code] = true;
            }

            pac->getEvent(event);
        }

        window.clear(sf::Color::White);

        if(menu.pick == 0)
        {
            menu.mouseMap = mouseMap;
            menu.keyMap = keyMap;
            menu.mousePos = mousePos;
            menu.draw(window);
        }
        else
        {
            switch(menu.pick)
            {
                case 1:
                {
                    pac = std::make_unique<PacView>(workdir, "p2_instruction_set.bin", "p2_equiphex.dat");
                    pac->read(pac_dir);
                    menu.pick = -1;
                    break;
                }

                case 2:
                {
                    pac = std::make_unique<PacView>(workdir, "p3_instruction_set.bin", "p3_equiphex.dat");
                    pac->read(pac_dir);
                    menu.pick = -1;
                    break;
                }

                case 3:
                {
                    pac = std::make_unique<PacView>(workdir, "p2_instruction_set.bin", "p1_equiphex.dat");
                    pac->read(pac_dir);
                    menu.pick = -1;
                    break;
                }
            }

            pac->mouseMap = mouseMap;
            pac->keyMap = keyMap;
            pac->mousePos = mousePos;
            pac->draw(window);
        }

        window.display();

        keyMap.clear();
    }

    return 0;
}

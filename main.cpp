#include <SFML/Graphics.hpp>
#include <iostream>
#include "PacView.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    cout << "PacViewer v3.0.0 by Owocek" << endl;

    if(argc > 1)
    {
        string workdir = argv[0];
        workdir = workdir.substr(0,workdir.find_last_of("\\")+1);
        PacView pac(workdir);
        pac.read(string(argv[1]));

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
                    pac.scroll += event.mouseWheel.delta * (-2);

                    if(pac.scroll <= 0)
                    pac.scroll = 0;
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
            }

            window.clear(sf::Color::White);
            pac.mouseMap = mouseMap;
            pac.keyMap = keyMap;
            pac.mousePos = mousePos;
            pac.draw(window);
            window.display();

            keyMap.clear();
        }

        return 0;
    }
    else
    {
        cout << "usage: PacViewer.exe <file>" << endl;
        return 0;
    }
}

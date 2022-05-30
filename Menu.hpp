#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>

class Menu
{
    public:
    sf::Font font;
    sf::Text info,info2,p1,p2,p3;

    int pick = 0;

    std::map<int,bool> mouseMap;
    std::map<int,bool> keyMap;
    sf::Vector2i mousePos;

    Menu(std::string workdir);
    void draw(sf::RenderWindow& window);
};

#endif

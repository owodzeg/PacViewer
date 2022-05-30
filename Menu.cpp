#include "Menu.hpp"

Menu::Menu(std::string workdir)
{
    font.loadFromFile(workdir+"p4kakupop-pro.ttf");

    info.setFont(font);
    info.setCharacterSize(24);
    info.setFillColor(sf::Color::Black);
    info.setString("Select the instruction set");
    info.setOrigin(info.getGlobalBounds().width/2,info.getGlobalBounds().height/2);

    info2.setFont(font);
    info2.setCharacterSize(22);
    info2.setFillColor(sf::Color::Black);
    info2.setString("(choosing an incorrect one can crash or hang the tool)");
    info2.setOrigin(info2.getGlobalBounds().width/2,info2.getGlobalBounds().height/2);

    p1.setFont(font);
    p1.setCharacterSize(32);
    p1.setFillColor(sf::Color::Black);
    p1.setString("Patapon 2");
    p1.setOrigin(p1.getGlobalBounds().width/2,p1.getGlobalBounds().height/2);

    p2.setFont(font);
    p2.setCharacterSize(32);
    p2.setFillColor(sf::Color::Black);
    p2.setString("Patapon 3");
    p2.setOrigin(p2.getGlobalBounds().width/2,p2.getGlobalBounds().height/2);

    p3.setFont(font);
    p3.setCharacterSize(32);
    p3.setFillColor(sf::Color::Black);
    p3.setString("Patapon 1");
    p3.setOrigin(p3.getGlobalBounds().width/2,p3.getGlobalBounds().height/2);
}

void Menu::draw(sf::RenderWindow& window)
{
    info.setPosition(window.getSize().x/2, 80);
    info2.setPosition(window.getSize().x/2, 110);
    p1.setPosition(window.getSize().x/6 * 2, 360);
    p2.setPosition(window.getSize().x/6 * 4, 360);
    p3.setPosition(window.getSize().x/6 * 3, 280);

    p1.setFillColor(sf::Color::Black);
    p2.setFillColor(sf::Color::Black);
    p3.setFillColor(sf::Color::Black);

    if(mousePos.x > p1.getPosition().x - p1.getGlobalBounds().width/2)
    {
        if(mousePos.x < p1.getPosition().x + p1.getGlobalBounds().width/2)
        {
            if(mousePos.y > p1.getPosition().y - p1.getGlobalBounds().height/2)
            {
                if(mousePos.y < p1.getPosition().y + p1.getGlobalBounds().height/2)
                {
                    p1.setFillColor(sf::Color::Green);

                    if(mouseMap[sf::Mouse::Left])
                    {
                            pick = 1;
                    }
                }
            }
        }
    }

    if(mousePos.x > p2.getPosition().x - p2.getGlobalBounds().width/2)
    {
        if(mousePos.x < p2.getPosition().x + p2.getGlobalBounds().width/2)
        {
            if(mousePos.y > p2.getPosition().y - p2.getGlobalBounds().height/2)
            {
                if(mousePos.y < p2.getPosition().y + p2.getGlobalBounds().height/2)
                {
                    p2.setFillColor(sf::Color::Green);

                    if(mouseMap[sf::Mouse::Left])
                    {
                            pick = 2;
                    }
                }
            }
        }
    }

    if(mousePos.x > p3.getPosition().x - p3.getGlobalBounds().width/2)
    {
        if(mousePos.x < p3.getPosition().x + p3.getGlobalBounds().width/2)
        {
            if(mousePos.y > p3.getPosition().y - p3.getGlobalBounds().height/2)
            {
                if(mousePos.y < p3.getPosition().y + p3.getGlobalBounds().height/2)
                {
                    p3.setFillColor(sf::Color::Green);

                    if(mouseMap[sf::Mouse::Left])
                    {
                            pick = 3;
                    }
                }
            }
        }
    }

    window.draw(info);
    window.draw(info2);
    window.draw(p1);
    window.draw(p2);
    window.draw(p3);
}

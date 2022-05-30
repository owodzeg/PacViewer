#ifndef PACMESSAGEBOX_HPP
#define PACMESSAGEBOX_HPP

#include <SFML/Graphics.hpp>

///a writeable message box (can be just informational too if i code it like that)
///main purpose -> edit pac and be able to display some cool stuff
class PacMessageBox
{
    public:
    sf::RectangleShape shadow; ///shadow on the instructions

    sf::RectangleShape box; ///main box for stuff
    std::vector<sf::RectangleShape> button; ///buttons (accept/cancel/ok/whatever)
    sf::RectangleShape writebox; ///box to write text into

    sf::Text t_header, t_writebox;
    std::vector<sf::Text> t_button;

    std::string writedata;

    bool is_writeable = false;

    PacMessageBox();
    void Create(sf::Font& font, std::string header, bool writeable, std::string preset_writeable, sf::Vector2f writeable_size, std::vector<std::string> button_text);
    void getEvent(sf::Event& event);
    void Draw(sf::RenderWindow& window);
};

#endif // PACMESSAGEBOX_HPP

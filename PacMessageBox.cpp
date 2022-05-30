#include "PacMessageBox.hpp"
#include <iostream>

PacMessageBox::PacMessageBox()
{

}

void PacMessageBox::Create(sf::Font& font, std::string header, bool writeable, std::string preset_writeable, sf::Vector2f writeable_size, std::vector<std::string> button_text)
{
    t_header.setFont(font);
    t_header.setCharacterSize(18);
    t_header.setFillColor(sf::Color::Black);
    t_header.setString(header);

    box.setFillColor(sf::Color(240,240,240));
    box.setOutlineThickness(4);
    box.setOutlineColor(sf::Color::Black);

    if(writeable)
    {
        writebox.setSize(writeable_size);
        writebox.setFillColor(sf::Color(222,222,222));
        writebox.setOutlineThickness(2);
        writebox.setOutlineColor(sf::Color::Black);

        t_writebox.setFont(font);
        t_writebox.setCharacterSize(14);
        t_writebox.setFillColor(sf::Color::Black);
        t_writebox.setString(preset_writeable);

        is_writeable = true;
    }

    for(int i=0; i<button_text.size(); i++)
    {
        sf::Text t_btn;
        t_btn.setFont(font);
        t_btn.setCharacterSize(16);
        t_btn.setFillColor(sf::Color::Black);
        t_btn.setString(button_text[i]);
        t_btn.setOrigin(t_btn.getLocalBounds().width/2, t_btn.getLocalBounds().height/2);

        sf::RectangleShape btn;
        btn.setSize(sf::Vector2f(t_btn.getLocalBounds().width + 8, t_btn.getLocalBounds().height + 4));
        btn.setOrigin(btn.getSize().x/2, btn.getSize().y/2);
        btn.setFillColor(sf::Color(230,230,230));
        btn.setOutlineThickness(1);
        btn.setOutlineColor(sf::Color::Black);

        t_button.push_back(t_btn);
        button.push_back(btn);
    }

    writedata = preset_writeable;
}

void PacMessageBox::getEvent(sf::Event& event)
{
    std::cout << "I receive event" << std::endl;
}

void PacMessageBox::Draw(sf::RenderWindow& window)
{
    ///draw shadow
    shadow.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    shadow.setFillColor(sf::Color(0,0,0,48));
    window.draw(shadow);

    ///window center
    float center_x=window.getSize().x/2, center_y=window.getSize().y/2;

    ///generic box size
    float box_x=0,box_y=0;
    box_x = t_header.getLocalBounds().width + 40;
    box_y = t_header.getLocalBounds().height + 40;

    ///if there's a writeable box, recalculate the generic box size
    if(is_writeable)
    {
        ///wrap pac text
        std::string rawstring = "";

        for(int r=0; r<writedata.size(); r+=2)
        {
            rawstring += std::string(1, writedata[r])+std::string(1, writedata[r+1])+" ";
        }

        std::string rawstring2 = "";

        for(int r=0; r<rawstring.size(); r++)
        {
            rawstring2 += rawstring[r];

            if((r+1) % 36 == 0)
            {
                rawstring2 += "\n";
            }
        }

        t_writebox.setString(rawstring2);

        ///write box size
        writebox.setSize(sf::Vector2f(writebox.getSize().x, t_writebox.getLocalBounds().height+8));

        box_x = writebox.getSize().x + 40;
        box_y = writebox.getSize().y + 40 + t_header.getLocalBounds().height;

        if(t_header.getLocalBounds().width > box_x)
        box_x = t_header.getLocalBounds().width + 20;
    }

    ///reserve space for buttons
    float btn_x=0, btn_y=0;

    for(int i=0; i<t_button.size(); i++)
    {
        btn_x += button[i].getSize().x + 30;

        if(btn_y < button[i].getSize().y)
        btn_y = button[i].getSize().y;
    }

    ///adjust generic box to buttons
    if(box_x < btn_x)
    box_x = btn_x + 20;

    box_y += btn_y;

    ///apply box config
    box.setSize(sf::Vector2f(box_x, box_y));
    box.setOrigin(box_x/2, box_y/2);
    box.setPosition(center_x, center_y);
    window.draw(box);

    ///apply header
    t_header.setOrigin(t_header.getLocalBounds().width/2, t_header.getLocalBounds().height/2);
    t_header.setPosition(center_x, center_y - box_y/2 + 10);
    window.draw(t_header);

    ///apply write box with text inside
    if(is_writeable)
    {
        writebox.setOrigin(writebox.getSize().x/2, writebox.getSize().y/2);
        writebox.setPosition(center_x, box.getPosition().y - box.getSize().y/2 + t_header.getLocalBounds().height + writebox.getSize().y/2 + 10);
        window.draw(writebox);

        t_writebox.setPosition(writebox.getPosition().x - writebox.getSize().x/2 + 2, writebox.getPosition().y - writebox.getSize().y/2 + 1);
        window.draw(t_writebox);
    }

    ///apply buttons
    for(int i=0; i<button.size(); i++)
    {
        button[i].setPosition(center_x - box_x/2 + (i * (box_x/button.size())) + (box_x/(button.size()*2)), box.getPosition().y + box.getSize().y/2 - btn_y);
        t_button[i].setPosition(center_x - box_x/2 + (i * (box_x/button.size())) + (box_x/(button.size()*2)), box.getPosition().y + box.getSize().y/2 - btn_y - 2);
        window.draw(button[i]);
        window.draw(t_button[i]);
    }
}

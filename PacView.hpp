#ifndef PACVIEW_HPP
#define PACVIEW_HPP

#include <SFML/Graphics.hpp>
#include <vector>

#include "Instruction.hpp"

class PacView
{
    public:
    struct LInstruction
    {
        bool l = false;
        uint8_t a,b,c,d;
        uint32_t i_size;
        std::string i_name;
        std::string ex_name;
        std::string i_desc;
        std::vector<std::string> param_type;
        std::vector<std::string> param_desc;
    };

    std::map<uint8_t,std::map<uint8_t,std::map<uint8_t,std::map<uint8_t,LInstruction>>>> INSSET; ///INSSET[A][B][C][D] = LInstruction

    std::string file;
    std::vector<Instruction> instructions;
    std::map<unsigned int,std::string> entities;
    std::map<unsigned int,std::string> equips;
    std::map<unsigned int,std::string> keybinds;

    std::map<int,bool> mouseMap;
    std::map<int,bool> keyMap;
    sf::Vector2i mousePos;

    int prev = 0;

    sf::Font font;
    sf::Text t_addr[32];
    sf::Text t_ins[32];
    sf::Text t_params[32];
    sf::RectangleShape i_select[32];
    sf::RectangleShape i_color[32];

    sf::RectangleShape box_div;
    sf::Text box_desc;

    sf::RectangleShape box_scroll;
    sf::RectangleShape box_scroll_bg;

    int arrow_sel = 0;
    int arrow_length = 0;
    sf::RectangleShape jump_arrow_begin;
    sf::RectangleShape jump_arrow_end;
    sf::RectangleShape jump_arrow;

    int ins_selected = -1;
    bool selected = false;

    int scroll = 0;
    int prevscroll = 0;
    bool scroll_clicked = false;

    int view = 1; ///0 - RAW, 1 - Parameters, 2 - Translated
    sf::Text t_view[3];

    PacView(std::string workdir);
    void read(std::string file);
    int getIDbyAddr(uint32_t addr);
    void save(std::string file);
    void draw(sf::RenderWindow& window);
};

#endif // PACVIEW_HPP

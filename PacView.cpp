#include "PacView.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <windows.h>

using namespace std;

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

template<typename T>
std::string to_hexstring(T i, int s)
{
  std::stringstream stream;
  stream << std::setfill('0') << std::setw(s*2) << std::hex << i;
  return stream.str();
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

unsigned int hstoui(const string &str)
{
    unsigned long u = stoul(str, nullptr, 16);

    if (u > UINT_MAX)
    throw out_of_range(str);

    return u;
}

void copyToClipboard(std::string word)
{
    const char* output = word.c_str();
    const size_t len = strlen(output) + 1;
    HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), output, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}

PacView::PacView(std::string workdir)
{
    cout << "[PAC] Working directory: " << workdir << endl;
    font.loadFromFile(workdir+"arial.ttf");

    box_div.setSize(sf::Vector2f(2,640));
    box_div.setFillColor(sf::Color::Black);

    jump_arrow_begin.setFillColor(sf::Color(0,255,0,0));
    jump_arrow_end.setFillColor(sf::Color(0,255,0,0));
    jump_arrow.setFillColor(sf::Color(0,255,0,0));

    t_view[0].setFont(font);
    t_view[0].setCharacterSize(16);
    t_view[0].setFillColor(sf::Color::Black);
    t_view[0].setString("Raw Binary");
    t_view[0].setOrigin(t_view[0].getGlobalBounds().width/2,t_view[0].getGlobalBounds().height/2);

    t_view[1].setFont(font);
    t_view[1].setCharacterSize(16);
    t_view[1].setFillColor(sf::Color::Black);
    t_view[1].setString("Parameters");
    t_view[1].setOrigin(t_view[1].getGlobalBounds().width/2,t_view[1].getGlobalBounds().height/2);

    t_view[2].setFont(font);
    t_view[2].setCharacterSize(16);
    t_view[2].setFillColor(sf::Color::Black);
    t_view[2].setString("Translated");
    t_view[2].setOrigin(t_view[2].getGlobalBounds().width/2,t_view[2].getGlobalBounds().height/2);

    cout << "[PAC] Loading instruction set... ";

    ///Read the instruction set
    ifstream is(workdir+"instruction_set.bin");
    string buf;
    int ic = 0;

    while(getline(is,buf))
    {
        vector<string> p = split(buf,';');

        LInstruction tmp;
        tmp.l = true;
        tmp.a = hstoui(p[0]);
        tmp.b = hstoui(p[1]);
        tmp.c = hstoui(p[2]);
        tmp.d = hstoui(p[3]);
        tmp.i_size = hstoui(p[4]);
        tmp.i_name = p[5];
        tmp.ex_name = p[6];
        string dsc = p[7];
        replaceAll(dsc,"\\n","\n");
        replaceAll(dsc,"\\\"","\"");
        tmp.i_desc = dsc;

        cout << hex << int(tmp.a) << " " << int(tmp.b) << " " << int(tmp.c) << " " << int(tmp.d) << " " << dec << tmp.i_size << " " << tmp.i_name << " " << tmp.i_desc << endl;

        int param_size = atoi(p[8].c_str());

        vector<string> p_t;
        vector<string> p_d;

        for(int i=0; i<param_size; i++)
        {
            p_t.push_back(p[9+(i*2)]);
            p_d.push_back(p[10+(i*2)]);

            ///cout << " " << p[9+(i*2)] << " " << p[10+(i*2)];
        }

        tmp.param_type = p_t;
        tmp.param_desc = p_d;

        ///cout << " " << tmp.param_type.size() << " " << tmp.param_desc.size();

        ///cout << endl;

        //cout << tmp.a << " " << tmp.b << " " << tmp.c << " " << tmp.d << endl;
        INSSET[tmp.a][tmp.b][tmp.c][tmp.d] = tmp;
        ic++;
    }

    is.close();

    cout << ic << " instructions loaded." << endl;

    ifstream el(workdir+"entityhex.dat");

    while(getline(el,buf))
    {
        string id = buf.substr(0,buf.find_first_of(","));
        string name = buf.substr(buf.find_first_of(",")+1);
        entities[hstoui(id)] = name;
    }

    el.close();

    cout << "[PAC] Loaded entity list" << endl;

    ifstream eq(workdir+"equiphex.dat");

    while(getline(eq,buf))
    {
        string id = buf.substr(0,buf.find_first_of(","));
        string name = buf.substr(buf.find_first_of(",")+1);
        equips[hstoui(id)] = name;
    }

    eq.close();

    cout << "[PAC] Loaded equipment list" << endl;

    ifstream ek(workdir+"keybindhex.dat");

    while(getline(ek,buf))
    {
        string id = buf.substr(0,buf.find_first_of(","));
        string name = buf.substr(buf.find_first_of(",")+1);
        keybinds[hstoui(id)] = name;
    }

    ek.close();

    cout << "[PAC] Loaded keybind list" << endl;
}

void PacView::read(std::string file)
{
    cout << "[PAC] Reading " << file << endl;

    int pacs = filesize(file.c_str());
    int offset = 0;

    ifstream pacf(file, ios::binary);

    vector<unsigned char> binary_buff;

    ///last values
    uint8_t la=0, lb=0, lc=0, ld=0;

    while(offset < pacs)
    {
        //cout << "[PAC] Reading offset 0x" << hex << offset << " / 0x" << pacs << dec << endl;
        //cout << "[PAC] Last D was " << hex << int(ld) << dec << endl;
        uint8_t a,b,c,d;
        char bb;

        pacf.seekg(offset);
        pacf.read(reinterpret_cast<char*>(&a), sizeof(uint8_t));

        pacf.seekg(offset);
        pacf.get(bb);
        binary_buff.push_back(bb);

        pacf.seekg(offset+0x1);
        pacf.read(reinterpret_cast<char*>(&b), sizeof(uint8_t));

        pacf.seekg(offset+0x1);
        pacf.get(bb);
        binary_buff.push_back(bb);

        pacf.seekg(offset+0x2);
        pacf.read(reinterpret_cast<char*>(&c), sizeof(uint8_t));

        pacf.seekg(offset+0x2);
        pacf.get(bb);
        binary_buff.push_back(bb);

        pacf.seekg(offset+0x3);
        pacf.read(reinterpret_cast<char*>(&d), sizeof(uint8_t));

        pacf.seekg(offset+0x3);
        pacf.get(bb);
        binary_buff.push_back(bb);

        int poff = offset;

        if((a == 0x25) && (c != 0x0) && (d <= 0x1))
        {
            if(binary_buff.size() > 4)
            {
                cout << "Some additional raw data detected! (0x" << hex << offset << dec << ")" << endl;
                ///Do something
                vector<unsigned char> raw = binary_buff;
                ///need to strip down next instruction
                raw.pop_back();
                raw.pop_back();
                raw.pop_back();
                raw.pop_back();

                Instruction ins;
                ins.setAddr(offset-raw.size());
                ins.setID(-255,-255);
                ins.f_name = "raw";
                ins.setRaw(raw);
                ins.setVisuals(a,b,c,d,ins.f_name,"Raw binary data","It is supposed to mean something.\nRaw data that doesn't match any\nof the instructions known to man.");
                instructions.push_back(ins);

                while(binary_buff.size() != 4)
                binary_buff.erase(binary_buff.begin());
            }

            Instruction ins;
            ins.setAddr(offset);
            ins.setID(b,c);

            //cout << a << " " << b << " " << c << " " << d << endl;

            if(INSSET[a][b][c][d].l)
            {
                if(INSSET[a][b][c][d].i_size == 0)
                {
                    ///If sizes are different for the same instruction

                    int tmp_off = offset+4;
                    uint8_t aa,cc,dd;

                    pacf.seekg(tmp_off);
                    pacf.read(reinterpret_cast<char*>(&aa), sizeof(uint8_t));

                    pacf.seekg(tmp_off+0x2);
                    pacf.read(reinterpret_cast<char*>(&cc), sizeof(uint8_t));

                    pacf.seekg(tmp_off+0x3);
                    pacf.read(reinterpret_cast<char*>(&dd), sizeof(uint8_t));

                    while(!((aa == 0x25) && (cc != 0x0) && (dd <= 0x1)))
                    {
                        pacf.seekg(tmp_off);
                        pacf.read(reinterpret_cast<char*>(&aa), sizeof(uint8_t));

                        pacf.seekg(tmp_off+0x2);
                        pacf.read(reinterpret_cast<char*>(&cc), sizeof(uint8_t));

                        pacf.seekg(tmp_off+0x3);
                        pacf.read(reinterpret_cast<char*>(&dd), sizeof(uint8_t));

                        if(!((aa == 0x25) && (cc != 0x0) && (dd <= 0x1)))
                        {
                            for(int e=0; e<4; e++)
                            {
                                char bu;

                                pacf.seekg(tmp_off+e);
                                pacf.get(bu);
                                binary_buff.push_back(bu);
                            }
                        }

                        tmp_off += 4;

                        if(tmp_off >= pacs)
                        {
                            break;
                        }
                    }
                }
                else
                {
                    ///If sizes are always the same for the instruction

                    for(int s=4; s<INSSET[a][b][c][d].i_size; s++)
                    {
                        pacf.seekg(poff+s);
                        pacf.get(bb);
                        binary_buff.push_back(bb);
                    }
                }

                ///cout << "[PAC] " << hex << "0x" << offset << dec << " " << INSSET[a][b][c][d].i_name << ", bb: " << binary_buff.size() << endl;
                ins.setRaw(binary_buff);
                ins.parseValues(INSSET[a][b][c][d].param_type,INSSET[a][b][c][d].param_desc,this);
                ins.setVisuals(a,b,c,d,INSSET[a][b][c][d].i_name,INSSET[a][b][c][d].ex_name,INSSET[a][b][c][d].i_desc);

                offset += binary_buff.size();
            }
            else
            {
                int tmp_off = offset+4;
                ///cout << "Start TMP_OFF: " << hex << tmp_off << dec << endl;

                uint8_t aa,cc;

                pacf.seekg(tmp_off);
                pacf.read(reinterpret_cast<char*>(&aa), sizeof(uint8_t));

                pacf.seekg(tmp_off+0x2);
                pacf.read(reinterpret_cast<char*>(&cc), sizeof(uint8_t));

                while(!((aa == 0x25) && (cc != 0x0)))
                {
                    pacf.seekg(tmp_off);
                    pacf.read(reinterpret_cast<char*>(&aa), sizeof(uint8_t));

                    pacf.seekg(tmp_off+0x2);
                    pacf.read(reinterpret_cast<char*>(&cc), sizeof(uint8_t));

                    if(!((aa == 0x25) && (cc != 0x0)))
                    {
                        for(int e=0; e<4; e++)
                        {
                            char bu;

                            pacf.seekg(tmp_off+e);
                            pacf.get(bu);
                            binary_buff.push_back(bu);
                        }
                    }

                    tmp_off += 4;

                    if(tmp_off >= pacs)
                    {
                        break;
                    }
                }

                ins.f_name = "unk"+to_hexstring(int(b),1)+"-"+to_hexstring(int(c),1);
                ins.setRaw(binary_buff);
                ins.setVisuals(a,b,c,d,ins.f_name,"Unknown function","Not yet documented or the behavior\nis unclear");

                ///cout << "[PAC] " << hex << "0x" << offset << dec << " " << INSSET[a][b][c][d].i_name << ", bb: " << binary_buff.size() << endl;
                offset += binary_buff.size();
            }

            instructions.push_back(ins);
            binary_buff.clear();
        }
        else
        {
            ///cout << "[PAC] Not a valid instruction" << endl;
        }

        ///cout << "[PAC] Reading finished" << endl;

        if(offset == poff)
        offset += 4;

        ///save last values
        la = a;
        lb = b;
        lc = c;
        ld = d;
    }

    pacf.close();
}

int PacView::getIDbyAddr(uint32_t addr)
{
    for(int i=0; i<instructions.size(); i++)
    {
        if(addr == instructions[i].f_addr)
        {
            return i;
        }
    }

    return -1;
}

void PacView::save(std::string file)
{
    cout << "Saving file " << file << endl;
    ofstream f(file,ios::binary);

    for(int i=0; i<instructions.size(); i++)
    {
        for(int a=0; a<instructions[i].raw_data.size(); a++)
        {
            f.put(instructions[i].raw_data[a]);
        }
    }

    f.close();
}

void PacView::draw(sf::RenderWindow& window)
{
    float s_height = float(window.getSize().y) / ((instructions.size()*24) / float(window.getSize().y));
    float s_pos = abs((scroll*24) / ((instructions.size()*24) / float(window.getSize().y)));

    if(s_height < 4)
    s_height = 4;

    box_scroll.setSize(sf::Vector2f(24,s_height));
    box_scroll.setPosition(616,s_pos);
    box_scroll.setFillColor(sf::Color(200,200,200,255));

    if(mouseMap[sf::Mouse::Left])
    {
        if(mousePos.x > 616)
        {
            if(mousePos.x <= 640)
            {
                if(mousePos.y >= s_pos)
                {
                    if(mousePos.y < s_pos+s_height)
                    {
                        scroll_clicked = true;
                    }
                }
            }
        }

        if(scroll_clicked)
        {
            scroll = round((mousePos.y * ((instructions.size()*24) / float(window.getSize().y))) / 24);

            if(scroll <= 0)
            scroll = 0;

            if(scroll >= instructions.size()-16)
            scroll = instructions.size()-16;
        }
    }
    else
    {
        scroll_clicked = false;
    }

    for(int i=0; i<32; i++) ///current viewed instructions, integrate with scroll bar
    {
        if(i+scroll < instructions.size())
        {
            t_addr[i].setFont(font);
            t_addr[i].setString(to_hexstring(instructions[i+scroll].f_addr,4));
            t_addr[i].setCharacterSize(16);
            t_addr[i].setFillColor(sf::Color::Black);
            t_addr[i].setPosition(0,i*24);

            t_ins[i].setFont(font);
            t_ins[i].setString(instructions[i+scroll].f_name);
            t_ins[i].setCharacterSize(16);
            t_ins[i].setFillColor(sf::Color::Black);
            t_ins[i].setPosition(144,i*24);

            t_params[i].setFont(font);
            t_params[i].setString(instructions[i+scroll].f_param);
            t_params[i].setCharacterSize(16);
            t_params[i].setFillColor(sf::Color::Black);
            t_params[i].setPosition(360,i*24);

            i_select[i].setSize(sf::Vector2f(616,24));
            i_select[i].setPosition(0,i*24);

            i_color[i].setSize(sf::Vector2f(616,24));
            i_color[i].setPosition(0,i*24);

            if(mouseMap[sf::Mouse::Left])
            {
                if(!scroll_clicked)
                {
                    if(mousePos.x >= 0)
                    {
                        if(mousePos.x <= 616)
                        {
                            if(mousePos.y >= i*24)
                            {
                                if(mousePos.y < i*24+24)
                                {
                                    instructions[prev].sel = false;
                                    prev = i+scroll;
                                    selected = true;
                                    ins_selected = i+scroll;
                                    instructions[i+scroll].sel = true;
                                    mouseMap[sf::Mouse::Left] = false;
                                }
                            }
                        }
                    }
                }
            }

            switch(instructions[i+scroll].cid)
            {
                case -255:
                i_color[i].setFillColor(sf::Color(80,80,80,255));
                break;

                case 0x0:
                i_color[i].setFillColor(sf::Color(230,230,230,255));
                break;

                case 0x1:
                i_color[i].setFillColor(sf::Color(255,179,186,255));
                break;

                case 0x2:
                i_color[i].setFillColor(sf::Color(255,223,186,255));
                break;

                case 0x3:
                i_color[i].setFillColor(sf::Color(255,255,186,255));
                break;

                case 0x4:
                i_color[i].setFillColor(sf::Color(186,255,201,255));
                break;

                case 0x5:
                i_color[i].setFillColor(sf::Color(186,225,255,255));
                break;

                case 0x6:
                i_color[i].setFillColor(sf::Color(223,247,251,255));
                break;

                case 0x7:
                i_color[i].setFillColor(sf::Color(232,213,248,255));
                break;

                case 0x8:
                i_color[i].setFillColor(sf::Color(255,234,198,255));
                break;

                case 0x9:
                i_color[i].setFillColor(sf::Color(218,251,206,255));
                break;

                case 0xA:
                i_color[i].setFillColor(sf::Color(0,255,161,255));
                break;

                case 0xB:
                i_color[i].setFillColor(sf::Color(0,188,212,255));
                break;

                case 0xC:
                i_color[i].setFillColor(sf::Color(255,138,128,255));
                break;

                case 0xD:
                i_color[i].setFillColor(sf::Color(234,128,252,255));
                break;

                case 0xE:
                i_color[i].setFillColor(sf::Color(255,209,128,255));
                break;

                case 0xF:
                i_color[i].setFillColor(sf::Color(238,254,238,255));
                break;

                case 0x10:
                i_color[i].setFillColor(sf::Color(180,150,178,255));
                break;

                case 0x11:
                i_color[i].setFillColor(sf::Color(90,106,142,255));
                break;

                case 0x12:
                i_color[i].setFillColor(sf::Color(176,222,238,255));
                break;

                case 0x13:
                i_color[i].setFillColor(sf::Color(170,170,170,255));
                break;

                case 0x14:
                i_color[i].setFillColor(sf::Color(51,201,223,255));
                break;

                case 0x15:
                i_color[i].setFillColor(sf::Color(58,125,199,255));
                break;

                case 0x16:
                i_color[i].setFillColor(sf::Color(244,100,136,255));
                break;

                case 0x17:
                i_color[i].setFillColor(sf::Color(255,249,110,255));
                break;

                case 0x18:
                i_color[i].setFillColor(sf::Color(246,93,93,255));
                break;

                case 0x19:
                i_color[i].setFillColor(sf::Color(2,199,115,255));
                break;

                case 0x1A:
                i_color[i].setFillColor(sf::Color(36,92,1,255));
                break;

                case 0x1B:
                i_color[i].setFillColor(sf::Color(196,251,73,255));
                break;

                case 0x1C:
                i_color[i].setFillColor(sf::Color(86,74,25,255));
                break;

                case 0x1D:
                i_color[i].setFillColor(sf::Color(255,132,30,255));
                break;

                case 0x1E:
                i_color[i].setFillColor(sf::Color(91,28,216,255));
                break;

                case 0x1F:
                i_color[i].setFillColor(sf::Color(198,135,234,255));
                break;
            }

            if(instructions[i+scroll].sel)
            {
                i_select[i].setFillColor(sf::Color(0,0,0,32));

                box_desc.setFont(font);
                box_desc.setCharacterSize(14);
                box_desc.setFillColor(sf::Color::Black);
                box_desc.setPosition(646,24);

                if(view == 0)
                {
                    string rawstring = "";

                    for(int r=0; r<instructions[i+scroll].raw_data.size(); r++)
                    {
                        rawstring += to_hexstring(int(instructions[i+scroll].raw_data[r]),1)+" ";
                    }

                    string rawstring2 = "";

                    for(int r=0; r<rawstring.size(); r++)
                    {
                        rawstring2 += rawstring[r];

                        if((r+1) % 36 == 0)
                        {
                            rawstring2 += "\n";
                        }
                    }

                    box_desc.setString(rawstring2);

                    if(keyMap[sf::Keyboard::R])
                    {
                        copyToClipboard(rawstring);
                        cout << "Copied RAW to clipboard" << endl;
                    }
                }
                else if(view == 1)
                {
                    box_desc.setString(instructions[i+scroll].f_desc);

                    if(keyMap[sf::Keyboard::R])
                    {
                        copyToClipboard(instructions[i+scroll].f_desc);
                        cout << "Copied DESC to clipboard" << endl;
                    }
                }
                else if(view == 2)
                {
                    box_desc.setString(instructions[i+scroll].f_desc_translated);

                    if(keyMap[sf::Keyboard::R])
                    {
                        copyToClipboard(instructions[i+scroll].f_desc_translated);
                        cout << "Copied TRA to clipboard" << endl;
                    }
                }
            }
            else
            {
                i_select[i].setFillColor(sf::Color(0,0,0,0));
            }

            window.draw(i_color[i]);
            window.draw(i_select[i]);

            window.draw(t_addr[i]);
            window.draw(t_ins[i]);
            window.draw(t_params[i]);
        }
    }

    if(selected)
    {
        if(instructions[ins_selected].pointer_addr != 0x1)
        {
            int dest = getIDbyAddr(instructions[ins_selected].pointer_addr);

            int diff = dest - (ins_selected);

            arrow_length = diff*24;
            arrow_sel = ins_selected;

            jump_arrow_begin.setFillColor(sf::Color(0,255,0,255));
            jump_arrow_end.setFillColor(sf::Color(0,255,0,255));
            jump_arrow.setFillColor(sf::Color(0,255,0,255));

            if(keyMap[sf::Keyboard::Enter])
            {
                prevscroll = scroll;
                scroll = dest-1;

                keyMap[sf::Keyboard::Enter] = false;
            }
        }
        else
        {
            jump_arrow_begin.setFillColor(sf::Color(0,255,0,0));
            jump_arrow_end.setFillColor(sf::Color(0,255,0,0));
            jump_arrow.setFillColor(sf::Color(0,255,0,0));
        }
    }

    if(keyMap[sf::Keyboard::Escape])
    {
        scroll = prevscroll;
    }

    jump_arrow_begin.setSize(sf::Vector2f(16,4));
    jump_arrow_begin.setPosition(112,arrow_sel*24-(scroll*24)+12);

    jump_arrow_end.setSize(sf::Vector2f(16,4));
    jump_arrow_end.setPosition(112,arrow_sel*24-(scroll*24)+12+arrow_length);

    jump_arrow.setSize(sf::Vector2f(4,arrow_length));
    jump_arrow.setPosition(112,arrow_sel*24-(scroll*24)+12);

    //cout << "y pos: " << (arrow_sel*24)+(scroll*24)+12 << " arrow: " << arrow_sel << " scroll: " << scroll << endl;

    window.draw(jump_arrow);
    window.draw(jump_arrow_begin);
    window.draw(jump_arrow_end);

    box_scroll_bg.setSize(sf::Vector2f(24,640));
    box_scroll_bg.setFillColor(sf::Color(240,240,240,255));
    box_scroll_bg.setPosition(616,0);

    window.draw(box_scroll_bg);
    window.draw(box_scroll);

    box_div.setPosition(640,0);
    window.draw(box_div);
    window.draw(box_desc);

    t_view[0].setFillColor(sf::Color(100,100,100,255));
    t_view[1].setFillColor(sf::Color(100,100,100,255));
    t_view[2].setFillColor(sf::Color(100,100,100,255));

    t_view[view].setFillColor(sf::Color(0,0,0,255));

    t_view[0].setPosition(690,6);
    t_view[1].setPosition(798,4);
    t_view[2].setPosition(910,4);

    for(int i=0; i<3; i++)
    {
        //cout << "i: " << i << " " << t_view[i].getPosition().x - t_view[i].getGlobalBounds().width/2 << " " << t_view[i].getPosition().x + t_view[i].getGlobalBounds().width/2 << " " << t_view[i].getPosition().y - t_view[i].getGlobalBounds().height/2 << " " << t_view[i].getPosition().y + t_view[i].getGlobalBounds().height/2 << "   " << mousePos.x << " " << mousePos.y << endl;

        if(mousePos.x > t_view[i].getPosition().x - t_view[i].getGlobalBounds().width/2)
        {
            if(mousePos.x < t_view[i].getPosition().x + t_view[i].getGlobalBounds().width/2)
            {
                if(mousePos.y > t_view[i].getPosition().y - t_view[i].getGlobalBounds().height/2)
                {
                    if(mousePos.y < t_view[i].getPosition().y + t_view[i].getGlobalBounds().height/2)
                    {
                        if(mouseMap[sf::Mouse::Left])
                        {
                            view = i;
                        }
                    }
                }
            }
        }
    }

    window.draw(t_view[0]);
    window.draw(t_view[1]);
    window.draw(t_view[2]);

    if(keyMap[sf::Keyboard::S])
    {
        save("PACVIEWER.pac"); ///Temporary name, will open a save file dialog
    }
}

#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <vector>
#include <iostream>

class PacView;

class Instruction
{
    public:
    int cid = -1; ///Class ID
    int fid = -1; ///Function ID

    bool sel = false;

    uint32_t f_addr;
    uint32_t pointer_addr = 0x1;
    bool jump_with_return = false;

    std::string f_name;
    std::string f_exname;
    std::string f_desc;
    std::string f_desc_translated;
    std::string f_param;

    std::vector<unsigned char> raw_data; ///Store RAW bytes
    std::vector<std::string> ready_params; ///Parameters ready to show
    std::vector<std::string> ready_types; ///Types ready to show
    std::vector<std::string> internal_types; ///Internal name of the type (as defined in dict file)
    std::vector<std::string> ready_params_pre; ///Parameters ready to show (for preview)
    std::vector<std::string> ready_names; ///Preset understandable names for certain parameters
    std::vector<std::string> ready_trans_params; ///Preset understandable parameters for functions
    std::vector<uint32_t> ready_offsets; ///In-instruction offset for each value, helpful for shifting all the pointers

    /**
    -=- Value types -=-
    0 - uint32_t
    1 - float
    2 - string
    100 - uint32_t_P
    10xx - uint32_t_Tx
    20xx - Vx
    */

    Instruction();
    void setID(int c, int f);
    void setAddr(uint32_t addr);
    void parseValues(std::vector<std::string> vtypes, std::vector<std::string> vnames, PacView* pacview);
    void setRaw(std::vector<unsigned char> value);
    void setVisuals(uint8_t a, uint8_t b, uint8_t c, uint8_t d, std::string name, std::string exname, std::string desc);
};

#endif // INSTRUCTION_HPP

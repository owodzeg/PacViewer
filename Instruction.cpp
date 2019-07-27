#include "Instruction.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include "PacView.hpp"

using namespace std;

template<typename T>
std::string to_hstring(T i, int s, bool lead=true)
{
  std::stringstream stream;
  if(lead)
  stream << std::setfill('0') << std::setw(s*2);
  stream << std::hex << i;
  return stream.str();
}

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 2)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}


Instruction::Instruction()
{

}

void Instruction::setID(int c, int f)
{
    cid = c;
    fid = f;

    //cout << "[PAC] Instruction set to Class " << cid << " Function " << fid << endl;
}

void Instruction::setAddr(uint32_t addr)
{
    f_addr = addr;
}

void Instruction::parseValues(std::vector<std::string> vtypes, std::vector<std::string> vnames, PacView* pacview)
{
    ready_names = vnames;
    int binary_offset = 4; ///don't count the instruction header

    for(int i=0; i<vtypes.size(); i++)
    {
        internal_types.push_back(vtypes[i]);

        if(vtypes[i] == "uint32_t")
        {
            uint32_t num = (uint32_t)raw_data[binary_offset+3] << 24 | (uint32_t)raw_data[binary_offset+2] << 16 | (uint32_t)raw_data[binary_offset+1] << 8 | (uint32_t)raw_data[binary_offset+0];
            ready_params.push_back(to_hstring(num,4));
            ready_trans_params.push_back(to_hstring(num,4));
            ready_params_pre.push_back(to_hstring(num,4,false));
            ready_types.push_back("uint32_t");

            ready_offsets.push_back(binary_offset);
            binary_offset += 4;
        }
        else if(vtypes[i] == "uint32_t_P")
        {
            uint32_t num = (uint32_t)raw_data[binary_offset+3] << 24 | (uint32_t)raw_data[binary_offset+2] << 16 | (uint32_t)raw_data[binary_offset+1] << 8 | (uint32_t)raw_data[binary_offset+0];
            ready_params.push_back(to_hstring(num,4));
            ready_trans_params.push_back(to_hstring(num,4));
            ready_params_pre.push_back(to_hstring(num,4,false));
            ready_types.push_back("uint32_t");
            pointer_addr = num;

            ready_offsets.push_back(binary_offset);
            binary_offset += 4;
        }
        else if(vtypes[i].find("uint32_t_T") != std::string::npos)
        {
            string Vid = "V"+vtypes[i].substr(vtypes[i].find_last_of("T")+1);

            uint32_t num = (uint32_t)raw_data[binary_offset+3] << 24 | (uint32_t)raw_data[binary_offset+2] << 16 | (uint32_t)raw_data[binary_offset+1] << 8 | (uint32_t)raw_data[binary_offset+0];
            ready_params.push_back(to_hstring(num,4));
            ready_trans_params.push_back(to_hstring(num,4));
            ready_params_pre.push_back(to_hstring(num,4,false));

            if(num == 0x10) ///float
            {
                for(int ii=0; ii<vtypes.size(); ii++)
                {
                    if(vtypes[ii] == Vid)
                    {
                        vtypes[ii] = "float";
                    }
                }
            }
            else if(num == 0x2) ///integer
            {
                for(int ii=0; ii<vtypes.size(); ii++)
                {
                    if(vtypes[ii] == Vid)
                    {
                        vtypes[ii] = "uint32_t";
                    }
                }
            }

            ready_types.push_back("uint32_t");
            ready_offsets.push_back(binary_offset);
            binary_offset += 4;
        }
        else if(vtypes[i] == "float")
        {
            uint32_t num = (uint32_t)raw_data[binary_offset+3] << 24 | (uint32_t)raw_data[binary_offset+2] << 16 | (uint32_t)raw_data[binary_offset+1] << 8 | (uint32_t)raw_data[binary_offset+0];
            float dest = *((float*)&num);

            ready_params.push_back(to_string_with_precision(dest,6));
            ready_trans_params.push_back(to_string_with_precision(dest,6));
            ready_params_pre.push_back(to_string_with_precision(dest,6));
            ready_types.push_back("float");
            ready_offsets.push_back(binary_offset);
            binary_offset += 4;
        }
        else if(vtypes[i] == "string")
        {
            ///Read until null is found, end reading when amount of bytes is %4 = 0 (because its how it works)
            ready_offsets.push_back(binary_offset);

            char buf = 0xFF;
            string str = "";

            while(buf != 0x00)
            {
                buf = raw_data[binary_offset];

                if(buf != 0x00)
                str += buf;

                binary_offset++;
            }

            while(binary_offset%4 != 0)
            binary_offset++;

            ready_params.push_back(str);
            ready_trans_params.push_back(str);
            ready_params_pre.push_back(str);
            ready_types.push_back("string");
        }
        else if(vtypes[i] == "ENTITY_ID")
        {
            uint32_t num = (uint32_t)raw_data[binary_offset+3] << 24 | (uint32_t)raw_data[binary_offset+2] << 16 | (uint32_t)raw_data[binary_offset+1] << 8 | (uint32_t)raw_data[binary_offset+0];
            string entity = pacview->entities[num];

            ready_params.push_back(to_hstring(num,4));
            ready_params_pre.push_back(to_hstring(num,4,false));
            ready_trans_params.push_back(entity);
            ready_types.push_back("uint32_t");
            ready_offsets.push_back(binary_offset);
            binary_offset += 4;
        }
        else if(vtypes[i] == "EQUIP_ID")
        {
            uint32_t num = (uint32_t)raw_data[binary_offset+3] << 24 | (uint32_t)raw_data[binary_offset+2] << 16 | (uint32_t)raw_data[binary_offset+1] << 8 | (uint32_t)raw_data[binary_offset+0];
            string entity = pacview->equips[num];

            ready_params.push_back(to_hstring(num,4));
            ready_params_pre.push_back(to_hstring(num,4,false));
            ready_trans_params.push_back(entity);
            ready_types.push_back("uint32_t");
            ready_offsets.push_back(binary_offset);
            binary_offset += 4;
        }
    }
}

void Instruction::setRaw(std::vector<unsigned char> value)
{
    raw_data = value;
}

void Instruction::setVisuals(uint8_t a, uint8_t b, uint8_t c, uint8_t d, std::string name, std::string exname, std::string desc)
{
    f_name = name;
    f_exname = exname;

    if(name == "raw")
    f_desc = f_exname+"\n\n"+desc;
    else
    f_desc = f_exname+"\nClass "+to_hstring(int(b),1)+" Function "+to_hstring(int(c),1)+"\n("+to_hstring(int(a),1)+" "+to_hstring(int(b),1)+" "+to_hstring(int(c),1)+" "+to_hstring(int(d),1)+")\n\n"+desc;

    f_desc_translated = f_desc;

    if(ready_params.size() > 0)
    {
        f_desc += "\n\nAdditional parameters:\n";
        f_desc_translated += "\n\nAdditional parameters:\n";
    }
    else
    {
        if(cid == -255)
        {
            f_param = to_string(raw_data.size())+" bytes unknown";
        }
        else
        {
            if(raw_data.size() > 4)
            f_param = to_string(raw_data.size()-4)+" bytes unknown";
            else
            f_param = "no params";
        }
    }

    for(int i=0; i<ready_params.size(); i++)
    {
        f_desc += ready_types[i]+" = "+ready_params[i]+"\n";
        f_desc_translated += ready_names[i]+" = "+ready_trans_params[i]+"\n";
        f_param += ready_params_pre[i];

        if(i != ready_params.size()-1)
        f_param += ",";
    }

    if(f_param.size() > 24)
    {
        f_param = f_param.substr(0,24)+"...";
    }
}

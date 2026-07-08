#include "../include/processor.hpp"



std::vector<std::string> RISC_V::Processor::split (const std::string str, char delimiter)
{
    std::vector<std::string> outstring;
    std::string substring = "";

    if(delimiter == '\0')
    {
        for(char c : str)
        {
            outstring.push_back(std::string(1, c));
        }
    }
    else
    {
        for(char c : str)
        {
        if(c == delimiter)
        {
            substring += c;
        }
        else
        {
            outstring.push_back(substring);
            substring = "";
        }
        }
        if(!substring.empty())  // To include ending characters if any
        {
            outstring.push_back(substring);
        }
    }
    
    return outstring;
}

std::string RISC_V::Processor::pick_chars(const std::string str, int start_idx, int end_idx)
{
    if(start_idx < 0 && end_idx >= str.size())
    {
        throw std::invalid_argument("Index is out of bounds!");
    }
    
    int length = end_idx - start_idx + 1;
    return str.substr(start_idx, length);
}

RISC_V::Processor::Processor()
{
    pc = 0;
    registers.fill(0);
    std::ifstream file("../data/instructions.txt");

    if(!file.is_open())
    {
        std::cerr << "Error: Could not open file." << std::endl;
        return;
    }

    std::string line;

    while(std::getline(file, line))
    {
        memory.push_back(line);
    }
    file.close();
}

int main()
{
    RISC_V::Processor processor = RISC_V::Processor();
    return 0;
}   
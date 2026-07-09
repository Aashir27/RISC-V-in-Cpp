#include "../include/processor.hpp"



// std::vector<std::string> RISC_V::Processor::split (const std::string str, char delimiter)
// {
//     std::vector<std::string> outstring;
//     std::string substring = "";

//     if(delimiter == '\0')
//     {
//         for(char c : str)
//         {
//             outstring.push_back(std::string(1, c));
//         }
//     }
//     else
//     {
//         for(char c : str)
//         {
//         if(c == delimiter)
//         {
//             substring += c;
//         }
//         else
//         {
//             outstring.push_back(substring);
//             substring = "";
//         }
//         }
//         if(!substring.empty())  // To include ending characters if any
//         {
//             outstring.push_back(substring);
//         }
//     }
    
//     return outstring;
// }

// std::string RISC_V::Processor::pick_chars(const std::string str, int start_idx, int end_idx)
// {
//     if(start_idx < 0 && end_idx >= str.size())
//     {
//         throw std::invalid_argument("Index is out of bounds!");
//     }
    
//     int length = end_idx - start_idx + 1;
//     return str.substr(start_idx, length);
// }

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


uint64_t RISC_V::Processor::mux(uint64_t a, uint64_t b, int select_bit)
{
    if(select_bit == 0)
    {
        return a;
    }
    return b;
}

uint64_t RISC_V::Processor::alu(uint64_t op1, uint64_t op2, ALU_OP control_bits)
{
    switch (control_bits)
    {
        case ALU_OP::ADD: 
            return op1 + op2;
        case ALU_OP::AND: 
            return op1 & op2;
        case ALU_OP::MUL: 
            return op1 * op2;
        case ALU_OP::OR: 
            return op1 | op2;
        case ALU_OP::XOR: 
            return op1 ^ op2;
        case ALU_OP::SUB: 
            return op1 - op2; 

        default:
            return 0;
    }
}

std::vector<uint32_t> RISC_V::Processor::instruction_parser(uint32_t instruction)
{
    std::vector<uint32_t> fields;
    uint32_t opcode = instruction & 127;   
    uint32_t rd = (instruction >> 7) & 31;   // Shifting 7 bits to right, picking 5 bits as max value in 5 bits is 31 
    uint32_t funct3 = (instruction >> 12) & 7; 
    uint32_t rs1 = (instruction >> 15) & 31;
    uint32_t rs2 = (instruction >> 20) & 31;
    uint32_t funct7 = (instruction >> 25) & 127;
    fields.push_back(opcode);
    fields.push_back(rd);
    fields.push_back(funct3);
    fields.push_back(rs1);
    fields.push_back(rs2);
    fields.push_back(funct7);
    return fields;
}


int main()
{
    RISC_V::Processor processor = RISC_V::Processor();
    return 0;
}   
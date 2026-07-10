#include "../include/processor.hpp"


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
    uint32_t mem_line;
    while(std::getline(file, line))
    {
        mem_line = std::stoul(line, nullptr, 0);
        uint8_t byte0 = mem_line & 255;
        uint8_t byte1 = (mem_line >> 8) & 255;
        uint8_t byte2 = (mem_line >> 16) & 255;
        uint8_t byte3 = (mem_line >> 24) & 255;
        memory.push_back(byte0);
        memory.push_back(byte1);
        memory.push_back(byte2);
        memory.push_back(byte3);
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

uint32_t RISC_V::Processor::instruction_memory(uint64_t instruction_addr)
{
    if(pc + 3 > memory.size())
    {
        std::cerr << "Error: PC out of memory bounds!" << std::endl;
        return 0;
    }
    uint32_t byte0 = static_cast<uint32_t>(memory[instruction_addr]);
    uint32_t byte1 = static_cast<uint32_t>(memory[instruction_addr + 1]);
    uint32_t byte2 = static_cast<uint32_t>(memory[instruction_addr + 2]);
    uint32_t byte3 = static_cast<uint32_t>(memory[instruction_addr + 3]);
    uint32_t instruction = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
    return instruction;
}

uint64_t RISC_V::Processor::data_memory(uint64_t memory_addr, uint64_t write_data, int mem_write, int mem_read)
{
    if(mem_read == 1 && mem_write == 0)
    {
        uint64_t read_memory = 0;
        for(int i = 0; i < 8; i++)
        {
            read_memory |= (static_cast<uint64_t>(memory[memory_addr + i]) << (i * 8));
        }

        return read_memory;
    }
    else if(mem_read == 0 && mem_write == 1)
    {
        for(int i = 0; i < 8; i++)
        {
            memory[memory_addr + i] = static_cast<uint8_t>((write_data >> (i * 8)) & 255);
        }
        return 0;
    }
    return 0;
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

std::vector<uint8_t> RISC_V::Processor::control_unit(uint8_t opcode)
{   
    std::vector<uint8_t> signals;
    int ALU_Src = 0, mem_to_reg = 0, reg_write = 0, mem_read = 0, mem_write = 0, branch = 0, ALU_Op = 0;
    if(opcode == 51)
    {
        reg_write = 1;
        ALU_Op = 2;
    }
    else if(opcode == 19)
    {
        ALU_Src = 1;
        reg_write = 1;
        ALU_Op = 2;
    }
    else if(opcode == 3)
    {
        ALU_Src = 1;
        mem_to_reg = 1;
        reg_write = 1;
        mem_read = 1;
    }
    else if(opcode == 35)
    {
        ALU_Src = 1;
        mem_write = 1;
    }
    else if(opcode == 99)
    {
        branch = 1;
        ALU_Op = 1;
    }
    signals.push_back(branch);
    signals.push_back(mem_read);
    signals.push_back(mem_to_reg);
    signals.push_back(ALU_Op);
    signals.push_back(mem_write);
    signals.push_back(ALU_Src);
    signals.push_back(reg_write);\

    return signals;
}




int64_t RISC_V::Processor::imm_gen(uint32_t instruction, uint8_t opcode)
{
    int64_t imm_data = 0;
    uint32_t imm_11_5 = 0;
    uint32_t imm_4_0 = 0;
    uint32_t imm_12 = 0;
    uint32_t imm_10_5 = 0;
    uint32_t imm_4_1 = 0;
    uint32_t imm_11 = 0;
    
    if(opcode == 19 || opcode == 3) // I-type instructions (addi, ld)
    {
        imm_data = static_cast<int32_t>(instruction) >> 20;
    }
    else if(opcode == 35)  // S-type isntructions 
    { 
        imm_11_5 = (instruction >> 25) & 127;  
        imm_4_0 = (instruction >> 7) & 31;
        uint32_t combined = (imm_11_5 << 5) | imm_4_0;
        imm_data = static_cast<int32_t>(combined << 20) >> 20;
    }
    else if(opcode == 99)  // SB-type instructions
    {
        imm_12 = (instruction >> 31) & 1;
        imm_10_5 = (instruction >> 25) & 63;
        imm_4_1 = (instruction >> 8) & 15;
        imm_11 = (instruction >> 7) & 1;
        uint32_t combined = (imm_12 << 12) | (imm_11 << 11) | (imm_10_5 << 5) | (imm_4_1 << 1);
        imm_data = static_cast<int32_t>(combined << 19) >> 19;
    }
    return imm_data;
}

std::vector<uint64_t> RISC_V::Processor::register_file(uint64_t write_data, uint32_t rs1, uint32_t rs2, uint32_t rd, int reg_write)
{
    std::vector<uint64_t> read_data;
    if(reg_write && rd != 0)
    {
        registers[rd] = static_cast<int64_t>(write_data);
    }
    registers[0] = 0;

    uint64_t data1 = static_cast<uint64_t>(registers[rs1]);
    uint64_t data2 = static_cast<uint64_t>(registers[rs2]);
    read_data.push_back(data1);
    read_data.push_back(data2);

    return read_data;
}



int main()
{
    RISC_V::Processor processor = RISC_V::Processor();
    return 0;
}   
#include "../include/processor.hpp"

RISC_V::Processor::Processor()
{
    final_write_data = 0;
    pc = 0;
    registers.fill(0);
    std::ifstream file("../data/instructions.txt");

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file." << std::endl;
        return;
    }

    std::string line;
    uint32_t mem_line;
    while (std::getline(file, line))
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
    if (select_bit == 0)
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
    case ALU_OP::OR:
        return op1 | op2;
    case ALU_OP::SUB:
        return op1 - op2;
    default:
        return 0;
    }
}

uint32_t RISC_V::Processor::instruction_memory(uint64_t instruction_addr)
{
    if (pc + 3 > memory.size())
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
    // If store instruction needs more space, increase memory and fill new byte spaces with 0
    if (mem_write == 1 && (memory_addr + 7 >= memory.size()))
    {
        memory.resize(memory_addr + 8, 0); // Fills any gaps with safe 0s
    }

    if (mem_read == 1 && (memory_addr + 7 >= memory.size()))
    {
        return 0; // Return safe 0 instead of crashing with out-of-bounds reads
    }

    if (mem_read == 1 && mem_write == 0)
    {
        uint64_t read_memory = 0;
        for (int i = 0; i < 8; i++)
        {
            read_memory |= (static_cast<uint64_t>(memory[memory_addr + i]) << (i * 8));
        }

        return read_memory;
    }
    else if (mem_read == 0 && mem_write == 1)
    {
        for (int i = 0; i < 8; i++)
        {
            memory[memory_addr + i] = static_cast<uint8_t>((write_data >> (i * 8)) & 255);
        }
        return 0;
    }
    return 0;
}

uint64_t RISC_V::Processor::get_data_memory()
{
    return instruction_memory(pc);
}

std::vector<uint32_t> RISC_V::Processor::instruction_parser(uint32_t instruction)
{
    std::vector<uint32_t> fields;
    uint32_t opcode = instruction & 127;
    uint32_t rd = (instruction >> 7) & 31; // Shifting 7 bits to right, picking 5 bits as max value in 5 bits is 31
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

uint64_t RISC_V::Processor::program_counter(uint64_t pc_in, int reset)
{
    uint64_t pc_out = pc;
    if (reset)
    {
        return pc_out;
    }
    return pc_in;
}

uint64_t RISC_V::Processor::adder(uint64_t a, uint64_t b)
{
    return a + b;
}

std::vector<uint8_t> RISC_V::Processor::control_unit(uint8_t opcode)
{
    std::vector<uint8_t> signals;
    int ALU_Src = 0, mem_to_reg = 0, reg_write = 0, mem_read = 0, mem_write = 0, branch = 0, ALU_Op = 0;
    if (opcode == 51)
    {
        reg_write = 1;
        ALU_Op = 2;
    }
    else if (opcode == 19)
    {
        ALU_Src = 1;
        reg_write = 1;
        ALU_Op = 2;
    }
    else if (opcode == 3)
    {
        ALU_Src = 1;
        mem_to_reg = 1;
        reg_write = 1;
        mem_read = 1;
    }
    else if (opcode == 35)
    {
        ALU_Src = 1;
        mem_write = 1;
    }
    else if (opcode == 99)
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
    signals.push_back(reg_write);

    return signals;
}

RISC_V::ALU_OP RISC_V::Processor::alu_control(int alu_op, uint32_t funct3, uint32_t instruction)
{
    if (alu_op == 0)
    {
        return RISC_V::ALU_OP::ADD;
    }
    else if (alu_op == 1)
    {
        return RISC_V::ALU_OP::SUB;
    }
    else if (alu_op == 2)
    {
        uint8_t bit_30 = (instruction >> 30) & 1;
        if (bit_30 == 0)
        {
            switch (funct3)
            {
            case 0:
                return RISC_V::ALU_OP::ADD;

            case 6:
                return RISC_V::ALU_OP::OR;

            case 7:
                return RISC_V::ALU_OP::AND;
            }
        }
        else
        {
            return RISC_V::ALU_OP::SUB;
        }
    }
    return RISC_V::ALU_OP::ADD;
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

    if (opcode == 19 || opcode == 3) // I-type instructions (addi, ld)
    {
        imm_data = static_cast<int32_t>(instruction) >> 20;
    }
    else if (opcode == 35) // S-type isntructions
    {
        imm_11_5 = (instruction >> 25) & 127;
        imm_4_0 = (instruction >> 7) & 31;
        uint32_t combined = (imm_11_5 << 5) | imm_4_0;
        imm_data = static_cast<int32_t>(combined << 20) >> 20;
    }
    else if (opcode == 99) // SB-type instructions
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
    if (reg_write && rd != 0)
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

void RISC_V::Processor::step()
{
    uint32_t instruction = instruction_memory(pc);
    if (instruction == 0 && pc + 3 >= memory.size())
    {
        return;
    }
    std::vector<uint32_t> fields = instruction_parser(instruction);
    uint32_t opcode = fields[0];
    uint32_t rd = fields[1];
    uint32_t funct3 = fields[2];
    uint32_t rs1 = fields[3];
    uint32_t rs2 = fields[4];

    std::vector<uint8_t> signals = control_unit(opcode);
    uint8_t branch = signals[0];
    uint8_t mem_read = signals[1];
    uint8_t mem_to_reg = signals[2];
    uint8_t ALU_Op = signals[3];
    uint8_t mem_write = signals[4];
    uint8_t ALU_Src = signals[5];
    uint8_t reg_write = signals[6];

    uint64_t imm_data = imm_gen(instruction, opcode);

    std::vector<uint64_t> read_data = register_file(0, rs1, rs2, rd, 0);
    uint64_t data1 = read_data[0];
    uint64_t data2 = read_data[1];

    int ALU_Src_int = ALU_Src;
    uint64_t b = mux(data2, imm_data, ALU_Src_int);
    RISC_V::ALU_OP operation = alu_control(ALU_Op, funct3, instruction);
    uint64_t alu_result = alu(data1, b, operation);

    int mem_write_int = mem_write;
    int mem_read_int = mem_read;
    uint64_t data_mem_out = data_memory(alu_result, data2, mem_write_int, mem_read_int);

    int mem_to_reg_int = mem_to_reg;
    final_write_data = mux(alu_result, data_mem_out, mem_to_reg_int);
    int reg_write_int = reg_write;
    if (reg_write_int)
    {
        register_file(final_write_data, rs1, rs2, rd, 1);
    }

    uint64_t pc_plus_4 = adder(pc, 4);
    uint64_t pc_branch = adder(pc, imm_data);
    int branch_int = branch;
    int zero_signal = 0;
    if (alu_result == 0)
    {
        zero_signal = 1;
    }
    uint64_t next_pc = mux(pc_plus_4, pc_branch, branch_int & !zero_signal);
    pc = program_counter(next_pc, 0);
}

void RISC_V::Processor::dump_registers()
{
    std::cout << "\nREGISTER FILE SNAPSHOT:\n";
    std::cout << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << pc << std::dec << "\n\n";

    for (int i = 0; i < 32; i++)
    {
        const std::string register_name = "x" + std::to_string(i) + ":";
        std::cout << std::left << std::setw(4) << std::setfill(' ') << register_name
                  << std::right << std::dec << std::setw(12) << std::setfill('0') << registers[i]
                  << "    ";
                  
        if ((i + 1) % 4 == 0)
            std::cout << "\n";
    }
}

void RISC_V::Processor::dump_memory(uint64_t start_addr, uint64_t element_count)
{
    std::cout << "\nDATA MEMORY:\n";
    std::cout << element_count << " data items starting at address " << start_addr << ".\n\n";

    for (uint64_t i = 0; i < element_count; i++)
    {
        uint64_t current_addr = start_addr + (i * 8);

        if (current_addr + 7 >= memory.size())
        {
            std::cout << "Address 0x" << std::hex << current_addr << " is OUT OF BOUNDS!\n";
            break;
        }

        uint64_t value = 0;
        for (int b = 0; b < 8; b++)
        {
            value |= (static_cast<uint64_t>(memory[current_addr + b]) << (b * 8));
        }

        const std::string index_label = std::to_string(i);
        std::cout << "Index: "<< std::setfill(' ') << index_label << ", Address: "
                  << current_addr << ", Value: "
                  << static_cast<int64_t>(value) << "\n";
    }
}

bool RISC_V::Processor::is_running()
{
    if (pc + 3 >= memory.size())
    {
        return false;
    }

    if (instruction_memory(pc) == 0)
    {
        return false;
    }

    return true;
}

void RISC_V::Processor::load_test_data(uint64_t address, uint64_t value)
{
    data_memory(address, value, 1, 0);
}

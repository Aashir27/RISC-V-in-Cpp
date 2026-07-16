#pragma once
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <iomanip>

namespace RISC_V
{
    enum class ALU_OP : uint8_t
    {
        ADD,
        SUB,
        AND,
        OR
    };
    class Processor
    {
        private:
            std::array<int64_t, 32> registers;
            uint64_t pc;
            uint64_t alu(uint64_t op1, uint64_t op2, ALU_OP control_bits);
            uint64_t mux(uint64_t a, uint64_t b, int select_bit);
            uint64_t adder(uint64_t a, uint64_t b);
            std::vector<uint8_t> memory;
            uint32_t instruction_memory(uint64_t instruction_addr);
            std::vector<uint32_t> instruction_parser(uint32_t instruction);
            std::vector<uint8_t> control_unit(uint8_t opcode);
            RISC_V::ALU_OP alu_control(int alu_op, uint32_t funct3, uint32_t instruction);
            uint64_t program_counter(uint64_t pc_in, int reset);
            int64_t imm_gen(uint32_t instruction, uint8_t opcode);
            uint64_t data_memory(uint64_t memory_addr, uint64_t write_data, int mem_write, int mem_read);
            std::vector<uint64_t> register_file(uint64_t write_data, uint32_t rs1, uint32_t rs2, uint32_t rd, int reg_write); 
            uint64_t final_write_data;

        public:
            Processor();
            void step();
            void dump_registers();
            void dump_memory(uint64_t start_addr, uint64_t element_count);
            uint64_t get_instruction();
            uint64_t get_data_memory();
            bool is_running();
            void load_test_data(uint64_t address, uint64_t value);
    };
}

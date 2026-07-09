#pragma once
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cstdint>

namespace RISC_V
{
    enum class ALU_OP : uint8_t
    {
        ADD,
        SUB,
        AND,
        OR,
        MUL,
        XOR,
        SLT,
        SLTU
    };
    class Processor
    {
    private:
        std::array<int64_t, 32> registers;
        uint64_t pc;
        uint64_t alu(uint64_t op1, uint64_t op2, ALU_OP control_bits);
        uint64_t mux(uint64_t a, uint64_t b, int select_bit);
        std::vector<std::string> memory;
        std::vector<uint32_t> instruction_parser(uint32_t instruction);
        // std::vector<std::string> split (const std::string str, char delimiter);
        // std::string pick_chars (const std::string str, int start_idx, int end_idx);

    public:
        Processor();
    };
}

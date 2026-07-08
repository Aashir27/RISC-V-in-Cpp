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
    enum class ALU_OP : uint8_t {
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
            std::array<int32_t, 32> registers;
            uint32_t pc;
            std::vector<std::string> memory;
            std::vector<std::string> split (const std::string str, char delimiter);
            std::string pick_chars (const std::string str, int start_idx, int end_idx);
            
        public:
            Processor();
    };
}

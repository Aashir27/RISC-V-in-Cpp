#include <iostream>
#include <vector>
#include "../include/processor.hpp"

int main()
{
    RISC_V::Processor cpu;
    uint64_t base_array_addr = 1000;
    std::vector<int64_t> unsorted_array = {80, 20, 50, 10, 30};

    std::cout << "Loading " << unsorted_array.size() << " doublewords into RAM at address " << base_array_addr << "\n";
    for (size_t i = 0; i < unsorted_array.size(); i++) 
    {
        uint64_t element_addr = base_array_addr + (i * 8);
        uint64_t payload = static_cast<uint64_t>(unsorted_array[i]);
        cpu.load_test_data(element_addr, payload); 
    }

    cpu.dump_memory(base_array_addr, unsorted_array.size());
    cpu.dump_registers();
    int cycle_count = 0;

    while (cpu.is_running())
    {
        cycle_count++;
        cpu.step();
    }
    cpu.dump_registers();
    cpu.dump_memory(base_array_addr, unsorted_array.size());
    std::cout << "Core pipeline state execution successfully finalized.\n";
    return 0;
}

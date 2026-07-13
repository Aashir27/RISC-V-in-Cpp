#include <iostream>
#include <vector>
#include "../include/processor.hpp"

int main()
{
    std::cout << "====================================================================\n";
    std::cout << "                 RISC_V EMULATOR MAIN INTERCONNECT                  \n";
    std::cout << "====================================================================\n";

    // 1. Initialize CPU (Constructor runs exactly as you wrote it)
    RISC_V::Processor cpu;

    // 2. Pre-load your test array elements at memory address 1000
    uint64_t base_array_addr = 1000;
    std::vector<int64_t> unsorted_array = {80, 20, 50, 10, 30};

    std::cout << "[SYSTEM] Pre-loading " << unsorted_array.size() << " doublewords into RAM at address " << base_array_addr << "...\n";
    for (size_t i = 0; i < unsorted_array.size(); i++) {
        uint64_t element_addr = base_array_addr + (i * 8);
        uint64_t payload = static_cast<uint64_t>(unsorted_array[i]);
        
        // Clean public method call
        cpu.load_test_data(element_addr, payload); 
    }

    // 3. Print out the initial baseline state
    std::cout << "\n>>> INITIAL DIAGNOSTIC SNAPSHOT <<<";
    cpu.dump_memory(base_array_addr, unsorted_array.size());
    cpu.dump_registers();

    // 4. Run the closed-circuit continuous clock pipeline loop
    std::cout << "\n[SYSTEM] Triggering Closed-Circuit Clock Cycles...\n";
    int cycle_count = 0;

    // The single, pure running condition block
    while (cpu.is_running())
    {
        cycle_count++;
        cpu.step();
    }

    std::cout << "[SYSTEM] Hit trailing code payload boundary or memory limits. Halting execution.\n";

    // 5. Output final status reports
    std::cout << "\n====================================================================";
    std::cout << "\n>>> POST-EXECUTION DIAGNOSTIC SNAPSHOT (Total Cycles: " << cycle_count << ") <<<";
    std::cout << "\n====================================================================";
    cpu.dump_registers();
    cpu.dump_memory(base_array_addr, unsorted_array.size());

    std::cout << "[SYSTEM] Core pipeline state execution successfully finalized.\n";
    return 0;
}

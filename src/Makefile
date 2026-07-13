# ====================================================================
#                     RISC-V SIMULATOR WORKSPACE
# ====================================================================

# Compiler settings
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Target executable name
TARGET   := riscv_sim

# Source files and include paths
SRCS     := main.cpp processor.cpp
INCLUDES := -Iinclude

# Default rule: Compiles the emulator automatically
all: $(TARGET)

$(TARGET): $(SRCS)
	@echo "[BUILD] Compiling C++ state machine modules..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET)
	@echo "[SUCCESS] Project compiled successfully! Type './$(TARGET)' or 'make run' to execute."

# Shortcut rule: Compiles and immediately executes your pipeline
run: $(TARGET)
	@echo "[EXECUTE] Booting baseline core simulation loop..."
	@./$(TARGET)

# Shortcut rule: Wipes out the generated binary file to clear up space
clean:
	@echo "[CLEAN] Removing generated simulator binaries..."
	@rm -f $(TARGET)
	@echo "[CLEAN] Workspace is clear."

.PHONY: all run clean

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2


TARGET   := riscv_sim

SRCS     := main.cpp processor.cpp
INCLUDES := -Iinclude

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET)


run: $(TARGET)
	@./$(TARGET)

clean:
	@rm -f $(TARGET)

.PHONY: all run clean

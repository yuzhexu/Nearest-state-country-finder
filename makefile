# Makefile for load.cpp

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17

# Build target
TARGET = load

# Build and link the program
all: $(TARGET)

$(TARGET): load.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) load.cpp

# Clean up
clean:
	rm -f $(TARGET)

# Run the program with the specified input file
run: all
	./$(TARGET) ./input/input.txt

.PHONY: all clean run

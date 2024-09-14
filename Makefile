# Makefile

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++17

# Executable name
TARGET = decipherExec

# Source files
SRCS = main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target to build the executable
all: $(TARGET)

# Rule to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile the C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to run the program (input files are not passed since they are hardcoded)
run: $(TARGET)
	./$(TARGET)

# Clean up the compiled objects and the executable
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean run

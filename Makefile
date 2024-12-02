# Define the compiler
COMPILER = g++

# Define compiler flags
COMPILER_FLAGS = -g -std=c++11 -Wall -Werror

# Define the library flags
LIBS= -lsimlib -lm

# Define the source directory and files
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Define object files (one for each .cpp file)
OBJS = $(SRCS:.cpp=.o)

# Define the target executable
TARGET = main

# Default target to build
all: $(TARGET)

# Rule to link the object files to create the executable
$(TARGET): $(OBJS)
	$(COMPILER) $(COMPILER_FLAGS) -o $@ $^ $(LIBS)

# Rule to compile source files into object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET) results.out debug.out

rebuilt: clean all

run:
	./main

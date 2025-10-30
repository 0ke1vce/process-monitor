# Compiler
CXX = g++

# Compiler flags
# -std=c++17 is needed for <filesystem>
# -Wall enables all warnings
CXXFLAGS = -std=c++17 -Wall

# Linker flags
LDFLAGS = -lncurses

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Find all .cpp files in the source directory
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Generate object file names from source file names
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# The final executable name
TARGET = monitor

# Default rule: build the target
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Rule to compile a .cpp file into a .o object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Rule to clean up build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

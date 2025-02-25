# Compiler settings
CXX := g++
CXX_WIN := x86_64-w64-mingw32-g++  # Adjust for your MinGW cross-compiler
CXXFLAGS := -std=c++11 -Wall -Iinclude -Iexternal
LDFLAGS := 

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Output binaries
TARGET := $(BIN_DIR)/app
TARGET_WIN := $(BIN_DIR)/app.exe

# Source and object files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Default rule
all: linux

# Create necessary directories
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Build for Linux
linux: $(BIN_DIR) $(OBJ_DIR) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build for Windows (cross-compile)
windows: CXX := $(CXX_WIN)
windows: $(BIN_DIR) $(OBJ_DIR) $(TARGET_WIN)

$(TARGET_WIN): $(OBJECTS)
	$(CXX_WIN) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Cleanup
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all linux windows clean

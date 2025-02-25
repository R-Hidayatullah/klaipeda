# Compiler settings
CXX := g++
CXX_WIN := x86_64-w64-mingw32-g++  # Adjust for your MinGW cross-compiler

# Common compiler flags
CXXFLAGS := -std=c++11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
            -Iinclude -Iexternal -Iexternal/imgui-docking -Iexternal/imgui-docking/backends \
            -Iexternal/glad33core/include -Iexternal/stb-master  # ✅ Added stb-master

LDFLAGS := 

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
IMGUI_DIR := external/imgui-docking
GLAD_DIR := external/glad33core
STB_DIR := external/stb-master

# Output binaries
TARGET := $(BIN_DIR)/klaipeda
TARGET_WIN := $(BIN_DIR)/klaipeda.exe

# Source and object files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp) \
           $(wildcard $(IMGUI_DIR)/*.cpp) \
           $(wildcard $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp) \
           $(wildcard $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp) \
           $(GLAD_DIR)/src/glad.c \
           $(STB_DIR)/stb_vorbis.c

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
OBJECTS_WIN := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.win.o, $(SOURCES))

# Default rule
all: linux

# Create necessary directories
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Build for Linux
linux: LDFLAGS += -lglfw -lGL -ldl -lpthread
linux: $(BIN_DIR) $(OBJ_DIR) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build (Linux only)
debug: CXXFLAGS += -g -DDEBUG -O0
debug: linux

# Sanitized build (Linux only, for debugging memory issues)
sanitize: CXXFLAGS += -fsanitize=address,undefined -g -DDEBUG -O0
sanitize: LDFLAGS += -fsanitize=address,undefined
sanitize: linux

# Show compile info
info:
	@echo "Compiler: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Target: $(TARGET)"
	@echo "Target Windows: $(TARGET_WIN)"

# Build for Windows (cross-compile, fully static)
windows: CXX := $(CXX_WIN)
windows: CXXFLAGS := -std=c++11 -Wall -Iinclude -Iexternal -Iexternal/imgui-docking \
                      -Iexternal/imgui-docking/backends -Iexternal/glad33core/include -Iexternal/stb-master
windows: LDFLAGS := -static -static-libgcc -static-libstdc++ -lopengl32 -lgdi32 -lglfw3 -lpthread
windows: $(BIN_DIR) $(OBJ_DIR) $(TARGET_WIN)

$(OBJ_DIR)/%.win.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX_WIN) $(CXXFLAGS) -c $< -o $@

$(TARGET_WIN): $(OBJECTS_WIN)
	$(CXX_WIN) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Cleanup
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all linux windows clean debug sanitize info

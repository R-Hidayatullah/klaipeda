cmake_minimum_required(VERSION 3.10.0)
project(klaipeda VERSION 0.1.0 LANGUAGES C CXX)

# Set C++11 standard with strict ISO compliance
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(
    include
    external/glad33core/include
    external/glfw-3.4.bin.WIN64/include
    external/glm-1.0.1-light
    external/imgui-docking
    external/imgui-docking/backends
    external/stb-master
)

# Link directories (for GLFW and other compiled libraries)
link_directories(
    external/glfw-3.4.bin.WIN64/lib-mingw-w64  # Adjust based on your compiler (e.g., lib-vc2022 for MSVC)
)

# Add the executable
add_executable(klaipeda 
    external/imgui-docking/imgui_demo.cpp
    external/imgui-docking/imgui_draw.cpp
    external/imgui-docking/imgui_tables.cpp
    external/imgui-docking/imgui_widgets.cpp
    external/imgui-docking/imgui.cpp
    external/imgui-docking/backends/imgui_impl_glfw.cpp 
    external/imgui-docking/backends/imgui_impl_opengl3.cpp 
    external/glad33core/src/glad.c
    external/stb-master/stb_vorbis.c
    src/main.cpp)

# Link libraries
target_link_libraries(klaipeda
    glfw3
    opengl32
    pthread
    z
    m
)

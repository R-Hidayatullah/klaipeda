cmake_minimum_required(VERSION 3.10.0)
project(klaipeda VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    include
    external/glad33core/include
    external/glfw-3.4.bin.WIN64/include
    external/glm-1.0.1-light
    external/imgui-docking
    external/imgui-docking/backends
    external/stb-master
    external/zlib-1.3.1
)

link_directories(
    external/glfw-3.4.bin.WIN64/lib-mingw-w64
)

set(ZLIB_SRC
    external/zlib-1.3.1/adler32.c
    external/zlib-1.3.1/compress.c
    external/zlib-1.3.1/crc32.c
    external/zlib-1.3.1/deflate.c
    external/zlib-1.3.1/gzclose.c
    external/zlib-1.3.1/gzlib.c
    external/zlib-1.3.1/gzread.c
    external/zlib-1.3.1/gzwrite.c
    external/zlib-1.3.1/infback.c
    external/zlib-1.3.1/inffast.c
    external/zlib-1.3.1/inflate.c
    external/zlib-1.3.1/inftrees.c     
    external/zlib-1.3.1/trees.c
    external/zlib-1.3.1/uncompr.c
    external/zlib-1.3.1/zutil.c
)
add_library(zlib STATIC ${ZLIB_SRC})
target_include_directories(zlib PUBLIC external/zlib-1.3.1)


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
    src/main.cpp
)

target_link_libraries(klaipeda
    glfw3
    opengl32
    zlib
)

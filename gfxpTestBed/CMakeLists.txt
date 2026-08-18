
cmake_minimum_required(VERSION 3.20)

project(gfxpTestBed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME gfxpTestBed)

set(SRCS
        src/main.cpp

        src/graphics/renderer.hpp
        src/graphics/renderer.cpp
)

add_executable(${TARGET_NAME} ${SRCS})

target_include_directories(${TARGET_NAME} PRIVATE
        src
        src/graphics
        ../gfxp/
)

target_link_libraries(${TARGET_NAME}
        glfw
        gfxp
)

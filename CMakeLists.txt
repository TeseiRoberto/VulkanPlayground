
cmake_minimum_required(VERSION 3.20)

project(vulkanPlaygroundProject)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME vulkanPlayground)

set(SRCS
        src/main.cpp

        src/graphics/commonEnums.hpp
        src/graphics/commonEnums.cpp
        src/graphics/vulkanContext.hpp
        src/graphics/vulkanContext.cpp
        src/graphics/shader/vulkanShader.hpp
        src/graphics/shader/vulkanShader.cpp
        src/graphics/pipeline/graphicsPipelineBuilder.hpp
        src/graphics/pipeline/graphicsPipelineBuilder.cpp

        src/graphics/renderer.hpp
        src/graphics/renderer.cpp
)

add_executable(${TARGET_NAME} ${SRCS})
target_include_directories(${TARGET_NAME} PRIVATE src src/graphics ${Vulkan_INCLUDE_DIR} )
target_link_libraries(${TARGET_NAME} glfw ${Vulkan_LIBRARIES})

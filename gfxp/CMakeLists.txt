
cmake_minimum_required(VERSION 3.20)

project(gfxpLibrary)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME gfxp)

set(SRCS

        # =====================================================================
        # Public source files
        gfxpLog.hpp
        gfxpEnums.hpp
        gfxpHandleTypes.hpp

        iGraphicContext.hpp
        iGraphicContext.cpp

        shaders/shaderDescription.hpp
        pipelines/graphicsPipelineDescription.hpp

        # =====================================================================
        # Common internal source files
        internals/handleManager.hpp

        # =====================================================================
        # Vulkan backend specific source files
        internals/backends/vulkan/vulkanEnumTranslator.hpp
        internals/backends/vulkan/vulkanEnumTranslator.cpp

        internals/backends/vulkan/vulkanContext.hpp
        internals/backends/vulkan/vulkanContext.cpp

        internals/backends/vulkan/shaders/vulkanShader.hpp
        internals/backends/vulkan/shaders/vulkanShaderFactory.hpp
        internals/backends/vulkan/shaders/vulkanShaderFactory.cpp

        internals/backends/vulkan/pipelines/vulkanGraphicsPipeline.hpp
        internals/backends/vulkan/pipelines/vulkanGraphicsPipelineFactory.hpp
        internals/backends/vulkan/pipelines/vulkanGraphicsPipelineFactory.cpp
)

add_library(${TARGET_NAME} ${SRCS})
target_include_directories(${TARGET_NAME} PRIVATE ${Vulkan_INCLUDE_DIR} )
target_link_libraries(${TARGET_NAME} ${Vulkan_LIBRARIES})

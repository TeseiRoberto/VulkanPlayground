
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
        shaders/shaderDescription.cpp
        pipelines/graphicsPipelineDescription.hpp
        pipelines/graphicsPipelineDescription.cpp

        # =====================================================================
        # Common internal source files
        internals/gpuResourceHandleManager.hpp

        # =====================================================================
        # Vulkan backend specific source files
        internals/backend/vulkan/vulkanEnumTranslator.hpp
        internals/backend/vulkan/vulkanEnumTranslator.cpp

        internals/backend/vulkan/vulkanContext.hpp
        internals/backend/vulkan/vulkanContext.cpp

        internals/backend/vulkan/buffers/vulkanBuffer.hpp
        internals/backend/vulkan/buffers/vulkanBufferFactory.hpp
        internals/backend/vulkan/buffers/vulkanBufferFactory.cpp

        internals/backend/vulkan/shaders/vulkanShader.hpp
        internals/backend/vulkan/shaders/vulkanShaderFactory.hpp
        internals/backend/vulkan/shaders/vulkanShaderFactory.cpp

        internals/backend/vulkan/pipelines/vulkanGraphicsPipeline.hpp
        internals/backend/vulkan/pipelines/vulkanGraphicsPipelineFactory.hpp
        internals/backend/vulkan/pipelines/vulkanGraphicsPipelineFactory.cpp
)

add_library(${TARGET_NAME} ${SRCS})
target_include_directories(${TARGET_NAME} PRIVATE ./ ${Vulkan_INCLUDE_DIR} )
target_link_libraries(${TARGET_NAME} ${Vulkan_LIBRARIES})


/**
 * @file vulkanShader.hpp
 * Defines the vp::VulkanShader class
*/

#ifndef VP_VULKAN_SHADER_H
#define VP_VULKAN_SHADER_H

#include <cstdint>
#include <vector>
#include <fstream>
#include <filesystem>
#include <vulkan/vulkan.h>

#include "log.hpp"
#include "vulkanContext.hpp"

namespace vp {


        /**
         * @class VulkanShader
         * Models a, single stage, SPIR-V shader that can be used to build a Vulkan graphics pipeline
        */
        class VulkanShader {
        public:

                /**
                 * @enum Type
                 * Enumerate the supported types of shader
                */
                enum class Type : uint32_t {
                        UNKNOWN = 0,            ///< Unknown shader type, shader is not valid
                        VERTEX,                 ///< Vertex shader
                        FRAGMENT,               ///< Fragment shader
                };

                                        VulkanShader(VulkanContext* context);
                                        ~VulkanShader();

                // Disable copies
                                        VulkanShader(const VulkanShader& other) = delete;
                VulkanShader&           operator = (const VulkanShader& other) = delete;

                inline bool             isValid() const { return (m_type != Type::UNKNOWN && m_module != VK_NULL_HANDLE); }

                bool                    loadFromSource(const std::filesystem::path& filePath, Type type);
                bool                    loadFromBinary(const std::filesystem::path& filePath, Type type);

                void                    unload();

                inline Type             getType() const         { return m_type; }
                inline VkShaderModule   getModule() const       { return m_module; }

        private:
                std::vector<char>       loadFileContent(const std::filesystem::path& filePath);

                bool                    createModule(const std::vector<char>& shaderBytecode, Type type);
                void                    destroyModule();


                VulkanContext*          m_context = nullptr;            ///< Vulkan context to which the shader is related
                Type                    m_type = Type::UNKNOWN;         ///< Type of shader
                VkShaderModule          m_module = VK_NULL_HANDLE;      ///< Handle to the shader module
        };

}

#endif // VP_VULKAN_SHADER_H

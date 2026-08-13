
/**
 * @file vulkanShader.hpp
 * Declares the vp::VulkanShader class
*/

#ifndef VP_VULKAN_SHADER_H
#define VP_VULKAN_SHADER_H

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <vulkan/vulkan.h>

#include "log.hpp"
#include "commonEnums.hpp"
#include "vulkanContext.hpp"

namespace vp {


        /**
         * @class VulkanShader
         * Models a, single stage, SPIR-V shader that can be used to build a Vulkan graphics pipeline
        */
        class VulkanShader {
        public:
                                                VulkanShader(VulkanContext* context);
                                                ~VulkanShader();

                                                // Disable copies
                                                VulkanShader(const VulkanShader& other) = delete;
                VulkanShader&                   operator = (const VulkanShader& other) = delete;

                inline bool                     isValid() const { return (m_type != ShaderType::UNKNOWN_SHADER && m_module != VK_NULL_HANDLE); }

                bool                            loadFromSource(const std::filesystem::path& filePath, ShaderType type, const std::string& entryPointName = "main");
                bool                            loadFromBinary(const std::filesystem::path& filePath, ShaderType type, const std::string& entryPointName = "main");

                void                            unload();

                inline ShaderType               getType() const         { return m_type; }
                inline VkShaderModule           getModule() const       { return m_module; }
                inline const std::string&       getEntryPoint() const   { return m_entryPointName; }

                void                            describeVertexBinding(uint32_t binding, VertexInputRate inputRate, size_t stride);
                void                            describeVertexAttribute(uint32_t binding, uint32_t location, VertexAttributeType attributeType, size_t offset);

                inline std::vector<VkVertexInputBindingDescription>     getVertexBindingsDescriptions() const      { return m_bindingsDescriptions; }
                inline std::vector<VkVertexInputAttributeDescription>   getVertexAttributesDescriptions() const    { return m_attributesDescriptions; }

        private:
                std::vector<char>               loadFileContent(const std::filesystem::path& filePath);

                bool                            createModule(const std::vector<char>& shaderBytecode, ShaderType type);
                void                            destroyModule();


                VulkanContext*                  m_context = nullptr;                            ///< Vulkan context to which the shader is related
                ShaderType                      m_type = ShaderType::UNKNOWN_SHADER;            ///< Type of shader
                VkShaderModule                  m_module = VK_NULL_HANDLE;                      ///< Handle to the shader module
                std::string                     m_entryPointName = "";                          ///< Name of the entry point function of the shader

                std::vector<VkVertexInputBindingDescription>    m_bindingsDescriptions;         ///< Structs wich describes the vertex bindings
                std::vector<VkVertexInputAttributeDescription>  m_attributesDescriptions;       ///< Structs wich describes the vertex attributes used by the shader
        };

}

#endif // VP_VULKAN_SHADER_H

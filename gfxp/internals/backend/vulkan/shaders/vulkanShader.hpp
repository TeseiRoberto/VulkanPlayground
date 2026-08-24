
/**
 * @file vulkanShader.hpp
 * Declares the gfxp::backend::VulkanShader struct
*/

#ifndef GFXP_BACKEND_VULKAN_SHADER_H
#define GFXP_BACKEND_VULKAN_SHADER_H

#include <vector>
#include <string>
#include <vulkan/vulkan.h>

#include "gfxpEnums.hpp"

namespace gfxp::backend {

        class VulkanContext;


        /**
         * @struct VulkanShader
         * POD struct that models a shader object for the Vulkan API backend
        */
        struct VulkanShader {
                gfxp::ShaderType        type = gfxp::ShaderType::UNKNOWN_SHADER;                ///< Type of shader
                VkShaderModule          handle = VK_NULL_HANDLE;                                ///< Handle to the shader module
                std::string             entryPointName = "main";                                ///< Name of the entry point function of the shader

                std::vector<VkVertexInputBindingDescription>    vrtxBindingsDescriptions;       ///< Structs wich describes the vertex bindings (used only for vertex shaders)
                std::vector<VkVertexInputAttributeDescription>  vrtxAttributesDescriptions;     ///< Structs wich describes the vertex attributes used by the shader (used only for vertex shaders)

                VulkanContext&          context;                                                ///< Context to which the shader belongs to


                /**
                 * @brief VulkanShader
                 * Struct constructor
                 * @param context Graphic context that owns the shader object
                */
                VulkanShader(VulkanContext& context) : context(context)
                {}


                /**
                 * @brief VulkanShader::isValid
                 * Utility method to check if the object is correctly initialized or not
                */
                inline bool isValid()
                {
                        return (handle != VK_NULL_HANDLE);
                }
        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_SHADER_H

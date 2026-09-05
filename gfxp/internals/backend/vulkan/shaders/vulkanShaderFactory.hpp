
/**
 * @file vulkanShaderFactory.hpp
 * Declares the gfxp::backend::VulkanShaderFactory class
*/

#ifndef GFXP_BACKEND_VULKAN_SHADER_FACTORY_H
#define GFXP_BACKEND_VULKAN_SHADER_FACTORY_H

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <vulkan/vulkan.h>

#include "shaders/shaderDescription.hpp"
#include "internals/backend/vulkan/vulkanDevice.hpp"
#include "internals/backend/vulkan/vulkanEnumTranslator.hpp"
#include "vulkanShader.hpp"

namespace gfxp::backend {


        /**
         * @class VulkanShaderFactory 
         * Class responsible for the creation and destruction of VulkanShader objects
        */
        class VulkanShaderFactory {
        public:
                explicit                                VulkanShaderFactory(VulkanDevice& device) : m_device(device) {}
                                                        ~VulkanShaderFactory() = default;

                VulkanShader*                           createShader(const gfxp::ShaderDescription& shaderDesc);
                static void                             destroyShader(VulkanShader*& shader);

        private:
                std::vector<char>                       loadFileContent(const std::filesystem::path& filePath);

                VkShaderModule                          createShaderModule(const std::vector<char>& shaderBytecode);
                static void                             destroyShaderModule(VulkanDevice& device, VkShaderModule& handle);

                std::vector<VkVertexInputBindingDescription>    extractVertexBindingsDescriptions(const ShaderDescription& shaderDesc);
                std::vector<VkVertexInputAttributeDescription>  extractVertexAttributesDescriptions(const ShaderDescription& shaderDesc);


                VulkanDevice&                          m_device;                ///< Device for which the factory will create resources for
        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_SHADER_FACTORY_H

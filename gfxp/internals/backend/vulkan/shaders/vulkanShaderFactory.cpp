
/**
 * @file vulkanShaderFactory.cpp
 * Implements the gfxp::backend::VulkanShaderFactory class
*/

#include "vulkanShaderFactory.hpp"

namespace gfxp::backend {


        /**
         * @brief VulkanShaderFactory::createShader
         * Tries to create a valid VulkanShader object according to the given shader description
         * @param shaderDesc Struct that describes the properties of the shader
         * @return Pointer to a valid VulkanShader object on success, nullptr otherwise
        */
        VulkanShader* VulkanShaderFactory::createShader(const gfxp::ShaderDescription& shaderDesc)
        {
                if( !m_context.isInit() )
                {
                        LOG_ERROR("VulkanShaderFactory::createShader() failed: VulkanContext is not initialized!");
                        return nullptr;
                }

                if(shaderDesc.getType() == gfxp::ShaderType::UNKNOWN)
                {
                        LOG_ERROR("VulkanShaderFactory::createShader() failed: UNKNOWN shader type specified!");
                        return nullptr;
                }

                if( shaderDesc.getEntryPointFunctionName().empty() )
                {
                        LOG_ERROR("VulkanShaderFactory::createShader() failed: shader's entry point function name is empty!");
                        return nullptr;
                }

                // Load shader from the source file
                std::vector<char> fileContent = loadFileContent(shaderDesc.getSourceFile());
                std::vector<char> shaderByteCode;

                if( shaderDesc.isBinary() )
                {
                        // File already contains the shader byte code
                        fileContent.swap(shaderByteCode);

                } else {
                        // Otherwise, try to compile shader from source code

                        // TODO: Add implementation...
                        LOG_ERROR("VulkanShaderFactory::createShader() failed: compilation of shader from source code is not implemented yet!");
                        return nullptr;
                }

                // Create the VkShaderModule
                VkShaderModule moduleHandle = createShaderModule(shaderByteCode);
                if(moduleHandle == VK_NULL_HANDLE)
                        return nullptr;

                // Create the shader object
                VulkanShader* shader = new VulkanShader(m_context);

                shader->handle = moduleHandle;
                shader->type = shaderDesc.getType();
                shader->entryPointName = shaderDesc.getEntryPointFunctionName();

                if(shader->type == gfxp::ShaderType::VERTEX_SHADER)
                {
                        shader->vrtxBindingsDescriptions = extractVertexBindingsDescriptions(shaderDesc);
                        shader->vrtxAttributesDescriptions = extractVertexAttributesDescriptions(shaderDesc);
                }

                return shader;
        }


        /**
         * @brief VulkanShaderFactory::destroyShader
         * Destroys and deallocates the given VulkanShader object
         * @param shader VulkanShader object that must be destroyed
        */
        void VulkanShaderFactory::destroyShader(VulkanShader*& shader)
        {
                if(shader == nullptr)
                {
                        LOG_ERROR("VulkanShaderFactory::destroyShader() failed: shader is not valid!");
                        return;
                }

                if( !shader->context.isInit() )
                {
                        LOG_ERROR("VulkanShaderFactory::destroyShader() failed: VulkanContext is not initialized!");
                        return;
                }

                destroyShaderModule(shader->context, shader->handle);

                delete shader;
                shader = nullptr;
        }


        /**
         * @brief VulkanShaderFactory::loadFileContent
         * Loads content of the specified file as binary data
         * @param filePath Path to file for which content shall be loaded
         * @return Data contained inside the specified file in binary format
        */
        std::vector<char> VulkanShaderFactory::loadFileContent(const std::filesystem::path& filePath)
        {
                if(filePath.empty())
                {
                        LOG_ERROR("VulkanShaderFactory::loadFileContent() failed, filepath is empty!");
                        return {};
                }

                // Try to open the file
                std::ifstream file(filePath, std::ios::binary | std::ios::ate);

                if( !file.is_open() )
                {
                        LOG_ERROR("VulkanShaderFactory::loadFileContent() failed, cannot open file \"%s\"!", filePath.c_str());
                        return {};
                }

                size_t fileSize = file.tellg();
                file.seekg(0);

                // Read file content
                std::vector<char> content;
                content.resize(fileSize);

                file.read(content.data(), fileSize);

                file.close();
                return content;
        }


        /**
         * @brief VulkanShaderFactory::createShaderModule
         * Tries to create a VkShaderModule from the given shader byte code
         * @param shaderByteCode Compiled byte code from which the module shall be built
         * @return A valid VkShaderModule handle on success, VK_NULL_HANDLE otherwise
        */
        VkShaderModule VulkanShaderFactory::createShaderModule(const std::vector<char>& shaderByteCode)
        {
                if(shaderByteCode.empty())
                {
                        LOG_ERROR("VulkanShaderFactory::createShaderModule() failed: shader's byte code is empty!");
                        return VK_NULL_HANDLE ;
                }

                VkShaderModule handle = VK_NULL_HANDLE;

                VkShaderModuleCreateInfo moduleInfo {};
                moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                moduleInfo.codeSize = shaderByteCode.size();
                moduleInfo.pCode = reinterpret_cast<const uint32_t*>( shaderByteCode.data() );

                if( vkCreateShaderModule(m_context.getLogicalDevice(), &moduleInfo, nullptr, &handle) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanShaderFactory::createShaderModule() failed: vkCreateShaderModule() failed!");
                        return VK_NULL_HANDLE;
                }

                return handle;
        }


        /**
         * @brief VulkanShaderFactory::destroyShaderModule
         * Destroys the VkShaderModule given
         * @param context Graphic context from which the shader module has been created
         * @param handle Handle to the shader module to be destroyed
        */
        void VulkanShaderFactory::destroyShaderModule(VulkanContext& context, VkShaderModule& handle)
        {
                if(handle == VK_NULL_HANDLE)
                        return;

                vkDestroyShaderModule(context.getLogicalDevice(), handle, nullptr);
                handle = VK_NULL_HANDLE;
        }


        /**
         * @brief VulkanShaderFactory::extractVertexBindingsDescriptions
         * Converts all API agnostic vertex bindings descriptions, contained in shaderDesc, to Vulkan specific structs
         * @param shaderDesc Struct that describes a vertex shader
         * @return Vector of Vulkan specific structs that describes the shader's vertex binding points
        */
        std::vector<VkVertexInputBindingDescription> VulkanShaderFactory::extractVertexBindingsDescriptions(const ShaderDescription& shaderDesc)
        {
                const std::vector< gfxp::ShaderDescription::VertexBindingDescription >& bindingsDesc = shaderDesc.getVertexBindingsDesc();

                std::vector<VkVertexInputBindingDescription> nativeBindingsDesc;
                nativeBindingsDesc.reserve( bindingsDesc.size() );

                for(const auto& desc : bindingsDesc)
                {
                        VkVertexInputBindingDescription nativeDesc {};

                        nativeDesc.binding = desc.slotIndex;
                        nativeDesc.stride = desc.stride;

                        if( !VulkanEnumTranslator::translate(desc.inputRate, nativeDesc.inputRate) )
                        {
                                LOG_ERROR("VulkanShaderFactory::extractVertexBindingsDescriptions() warning: failed to convert vertex input rate to native Vulkan enum, binding will be skipped!");
                                continue;
                        }

                        nativeBindingsDesc.push_back(nativeDesc);
                }

                return nativeBindingsDesc;
        }


        /**
         * @brief VulkanShaderFactory::extractVertexAttributesDescriptions
         * Converts all API agnostic vertex attributes descriptions, contained in shaderDesc, to Vulkan specific structs
         * @param shaderDesc Struct that describes a vertex shader
         * @return Vector of Vulkan specific structs that describes the shader's vertex attributes
        */
        std::vector<VkVertexInputAttributeDescription> VulkanShaderFactory::extractVertexAttributesDescriptions(const ShaderDescription& shaderDesc)
        {
                const std::vector< gfxp::ShaderDescription::VertexAttributeDescription >& attributesDesc = shaderDesc.getVertexAttributesDesc();

                std::vector<VkVertexInputAttributeDescription> nativeAttributesDesc;
                nativeAttributesDesc.reserve( attributesDesc.size() );

                for(const auto& desc : attributesDesc)
                {
                        VkVertexInputAttributeDescription nativeDesc {};

                        nativeDesc.location = desc.location;
                        nativeDesc.binding = desc.bindingSlotIndex;
                        nativeDesc.offset = desc.offset;

                        if( !VulkanEnumTranslator::translate(desc.dataType, nativeDesc.format) )
                        {
                                LOG_ERROR("VulkanShaderFactory::extractVertexAttributesDescriptions() warning: failed to convert vertex attribute data type to native Vulkan enum, attribute will be skipped!");
                                continue;
                        }

                        nativeAttributesDesc.push_back(nativeDesc);
                }

                return nativeAttributesDesc;
        }


} // namespace gfxp::backend


#include "vulkanShader.hpp"

namespace vp {


        /**
         * @brief VulkanShader::VulkanShader
         * Constructs an invalid VulkanShader instance
         * @param context Vulkan context to which the shader is related
        */
        VulkanShader::VulkanShader(VulkanContext* context) : m_context(context)
        {}


        /**
         * @brief VulkanShader::~VulkanShader
         * Destroys the VulkanShaderModule object if one has been constructed
        */
        VulkanShader::~VulkanShader()
        {
                unload();
        }


        /**
         * @brief VulkanShader::loadFromSource
         * Tries to load GLSL source code from file and to compile it into a 
         * valid SPIR-V shader
         * @param filePath Path to file that contains the shader source code
         * @param type Type of shader to be loaded
         * @return True on success, false otherwise
         * @warning NOT IMPLEMENTED YET, WHEN CALLED FALSE WILL BE RETURNED!!!
        */
        bool VulkanShader::loadFromSource(const std::filesystem::path& filePath, Type type)
        {
                // TODO: Add implementation...
                return false;
        }


        /**
         * @brief VulkanShader::loadFromBinary
         * Tries to load a SPIR-V shader from the specified file
         * @param filePath Path to file that contains the shader byte-code
         * @param type Type of shader to be loaded
         * @return True on success, false otherwise
        */
        bool VulkanShader::loadFromBinary(const std::filesystem::path& filePath, Type type)
        {
                if(type == Type::UNKNOWN)
                {
                        LOG_ERROR("VulkanShader::loadFromBinary() failed: UNKNOWN shader type specified!");
                        return false;
                }

                std::vector<char> fileContent = loadFileContent(filePath);
                createModule(fileContent, type);

                return (m_module != VK_NULL_HANDLE);
        }


        /**
         * @brief VulkanShader::unload
         * Destroys the shader module associated to the shader
        */
        void VulkanShader::unload()
        {
                destroyModule();
        }


        /**
         * @brief VulkanShader::loadFileContent
         * Loads content of the specified file as binary data
         * @param filePath Path to file for which content shall be loaded
         * @return Data contained inside the specified file in binary format
        */
        std::vector<char> VulkanShader::loadFileContent(const std::filesystem::path& filePath)
        {
                if(filePath.empty())
                {
                        LOG_ERROR("VulkanShader::loadFileContent() failed, filepath is empty!");
                        return {};
                }

                // Try to open the file
                std::ifstream file(filePath, std::ios::binary | std::ios::ate);

                if( !file.is_open() )
                {
                        LOG_ERROR("VulkanShader::loadFileContent() failed, cannot open file \"%s\"!", filePath.c_str());
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
         * @brief VulkanShader::createModule
         * Creates the VulkanShaderModule object
         * @param shaderByteCode ByteCode of the shader
         * @param type Type of shader module to be created
         * @return True on success, false on failure
        */
        bool VulkanShader::createModule(const std::vector<char>& shaderByteCode, Type type)
        {
                if(m_context == nullptr)
                {
                        LOG_ERROR("VulkanShader::createModule() failed: no VulkanContext is bound to the shader instance!");
                        return false;
                }

                if(shaderByteCode.empty())
                {
                        LOG_ERROR("VulkanShader::createModule() failed: file is empty!");
                        return false;
                }

                VkShaderModuleCreateInfo moduleInfo {};
                moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                moduleInfo.codeSize = shaderByteCode.size();
                moduleInfo.pCode = reinterpret_cast<const uint32_t*>( shaderByteCode.data() );

                if( vkCreateShaderModule(m_context->getLogicalDevice(), &moduleInfo, nullptr, &m_module) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanShader::createModule() failed: vkCreateShaderModule() failed!");
                        return false;
                }

                m_type = type;
                return true;
        }


        /**
         * @brief VulkanShader::destroyModule
         * Destroys the VulkanshaderModule object associated to the instance,
         * if one exists
        */
        void VulkanShader::destroyModule()
        {
                if( !isValid() )
                        return;

                vkDestroyShaderModule(m_context->getLogicalDevice(), m_module, nullptr);

                m_module = VK_NULL_HANDLE;
                m_type = Type::UNKNOWN;
        }


}

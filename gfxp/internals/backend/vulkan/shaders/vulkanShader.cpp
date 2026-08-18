
#include "vulkanShader.hpp"

namespace vp {


        /**
         * @brief VulkanShader::VulkanShader
         * Constructs an invalid VulkanShader instance
         * @param context Vulkan context to which the shader is related, such pointer must remain valid
         * until the shader instance is not destroyed
        */
        VulkanShader::VulkanShader(VulkanContext* context)
                : m_context(context)
                , m_type(ShaderType::UNKNOWN_SHADER)
        {}


        /**
         * @brief VulkanShader::~VulkanShader
         * Destroys the VkShaderModule object if one has been constructed
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
         * @param entryPointName Name of the entry point function of the shader
         * @return True on success, false otherwise
         * @warning NOT IMPLEMENTED YET, WHEN CALLED FALSE WILL BE RETURNED!!!
        */
        bool VulkanShader::loadFromSource(const std::filesystem::path& filePath, ShaderType type, const std::string& entryPointName)
        {
                // TODO: Add implementation...
                return false;
        }


        /**
         * @brief VulkanShader::loadFromBinary
         * Tries to load a SPIR-V shader from the specified file
         * @param filePath Path to file that contains the shader byte-code
         * @param type Type of shader to be loaded
         * @param entryPointName Name of the entry point function of the shader
         * @return True on success, false otherwise
        */
        bool VulkanShader::loadFromBinary(const std::filesystem::path& filePath, ShaderType type, const std::string& entryPointName)
        {
                if(type == ShaderType::UNKNOWN_SHADER)
                {
                        LOG_ERROR("VulkanShader::loadFromBinary() failed: UNKNOWN shader type specified!");
                        return false;
                }

                if(entryPointName.empty())
                {
                        LOG_WARN("VulkanShader::loadFromBinary() warning: shader entry point function name is empty, defaulting to \"main\"!");
                        m_entryPointName = "main";

                } else {
                        m_entryPointName = entryPointName;
                }

                std::vector<char> fileContent = loadFileContent(filePath);
                createModule(fileContent, type);

                return (m_module != VK_NULL_HANDLE);
        }


        /**
         * @brief VulkanShader::unload
         * Destroys the VkShaderModule associated to the shader and resets all shader's properties
        */
        void VulkanShader::unload()
        {
                destroyModule();

                m_entryPointName = "";
                m_bindingsDescriptions.clear();
                m_attributesDescriptions.clear();
        }


        /**
         * @brief VulkanShader::describeVertexBinding
         * @param binding Slot index of the vertex buffer from which data will be fetched
         * @param inputRate Way in which data shall be fetched from the buffer
         * @param stride Offset, expressed in bytes, between two consecutive "data entries" stored in the buffer
         * @note This method has no effect if the shader is not a vertex shader
        */
        void VulkanShader::describeVertexBinding(uint32_t binding, VertexInputRate inputRate, size_t stride)
        {
                if(m_type != ShaderType::VERTEX_SHADER)
                        return;

                VkVertexInputBindingDescription currBindingInfo {};

                currBindingInfo.binding         = binding;
                currBindingInfo.stride          = stride;

                if( !EnumTranslator::translate(inputRate, currBindingInfo.inputRate) )
                {
                        LOG_ERROR("VulkanShader::describeVertexBinding() failed: an unknown inputRate is given!");
                        return;
                }

                m_bindingsDescriptions.push_back(currBindingInfo);
        }


        /**
         * @brief VulkanShader::describeVertexAttribute
         * Describes one of the attributes that the vertex shader will receive as input
         * @param binding Slot index of the vertex buffer from which the attribute will be fetched
         * @param location Shader input location (slot index) to which the attribute is associated
         * @param type Data type of the attribute
         * @param offset Offset of this attribute, expressed in bytes, relative to the start of the "data entry" stored in the vertex buffer
         * @note This method has no effect if the shader is not a vertex shader
         * @note @a location must be unique even if multiple bindings are used
        */
        void VulkanShader::describeVertexAttribute(uint32_t binding, uint32_t location, VertexAttributeType attributeType, size_t offset)
        {
                if(m_type != ShaderType::VERTEX_SHADER)
                        return;

                VkVertexInputAttributeDescription currAttributeInfo {};

                currAttributeInfo.binding       = binding;
                currAttributeInfo.location      = location;
                currAttributeInfo.offset        = offset;

                if( !EnumTranslator::translate(attributeType, currAttributeInfo.format) )
                {
                        LOG_ERROR("VulkanShader::describeVertexAttribute() failed: an unknown attributeType is given!");
                        return;
                }

                m_attributesDescriptions.push_back(currAttributeInfo);
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
        bool VulkanShader::createModule(const std::vector<char>& shaderByteCode, ShaderType type)
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
         * Destroys the VkShaderModule object associated to the instance, if one exists
        */
        void VulkanShader::destroyModule()
        {
                if( !isValid() )
                        return;

                vkDestroyShaderModule(m_context->getLogicalDevice(), m_module, nullptr);

                m_module = VK_NULL_HANDLE;
                m_type = ShaderType::UNKNOWN_SHADER;
        }


}

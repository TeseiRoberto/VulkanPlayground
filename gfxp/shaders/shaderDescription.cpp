
/**
 * @file shaderDescription.cpp
 * Implements the gfxp::ShaderDescription class
*/

#include "shaderDescription.hpp"

namespace gfxp {


        /**
         * @brief ShaderDescription::ShaderDescription
         * Class constructor, creates a default description for a shader (@link see ShaderDescription::reset)
        */
        ShaderDescription::ShaderDescription()
        {
                reset();
        }


        /**
         * @brief ShaderDescription::setSourceFile
         * Sets the path to the file from which the shader source code, or the precompiled byte code, shall be loaded
         * @param filePath Path to the source file
         * @param isBinary Shall be true if filePath contains shader precompiled byte code, false otherwise
         * @param type Type of shader that is being described by this description instance
         * @return Reference to this description class instance so that methods can be concatenated
        */
        ShaderDescription& ShaderDescription::setSourceFile(const std::filesystem::path& filePath, bool isBinary, ShaderType type)
        {
                if(type == ShaderType::UNKNOWN)
                {
                        LOG_ERROR("ShaderDescription::setSourceFile() failed: given shader type is unknown!");
                        return *this;
                }

                m_isBinary = isBinary;
                m_filePath = filePath;
                m_shaderType = shaderType;

                return *this;
        }

        /**
         * @brief ShaderDescription::setEntryPointFunctionName
         * Sets the name of the function that shall be invoked as main when the shader is executed
         * @param Name of the shader entry point function
         * @return Reference to this description class instance so that methods can be concatenated
        */
        ShaderDescription& ShaderDescription::setEntryPointFunctionName(const std::string& name)
        {
                m_entryPointName = name;
                return *this;
        }


        /**
         * @brief ShaderDescription::describeVertexBinding
         * Describes a binding point (slot) of the shader for a vertex buffer
         * @param binding Numeric index of the binding point (slot)
         * @param inputRate Rate at which data shall be fetched from the vertex buffer
         * @param stride Bytes between two different "data entries" stored in the vertex buffer
         * @return Reference to this description class instance so that methods can be concatenated
         * @warning Calling this method on an instance that does not describe a vertex shader has no effect
        */
        ShaderDescription& ShaderDescription::describeVertexBinding(uint32_t binding, VertexInputRate inputRate, size_t stride)
        {
                if(m_shaderType != ShaderType::VERTEX:SHADER)
                {
                        LOG_WARN("ShaderDescription::describeVertexBinding() failed: the description class instance is not describing a vertex shader!");
                        return *this;
                }

                m_vertexBindingsDesc.emplace( binding, inputRate, stride );
                return *this;
        }


        /**
         * @brief ShaderDescription::describeVertexAttribute
         * Describes a vertex attribute taken as input by a vertex shader
         * @param binding Vertex binding point (slot) from which the attribute is fetched
         * @param location Unique numeric id used to identify the attribute (must be unique even if different binding points are used)
         * @param attributeType Data type of the vertex attribute
         * @param offset Offset of the attribute from the start of the "data entry" fetched from the vertex buffer
         * @return Reference to this description class instance so that methods can be concatenated
         * @warning Calling this method on an instance that does not describe a vertex shader has no effect
        */
        ShaderDescription& ShaderDescription::describeVertexAttribute(uint32_t binding, uint32_t location, VertexAttributeType attributeType, size_t offset)
        {
                if(m_shaderType != ShaderType::VERTEX:SHADER)
                {
                        LOG_WARN("ShaderDescription::describeVertexAttribute() failed: the description class instance is not describing a vertex shader!");
                        return *this;
                }

                m_vertexAttributesDesc.emplace( binding, location, attributeType, offset );
                return *this;
        }


        /**
         * @brief ShaderDescription::reset
         * Resets the description to the default invalid configuration.
         * A resetted, or default constructed, ShaderDescription is not valid to create any type of shader object
        */
        void ShaderDescription::reset()
        {
                m_isBinary = false;
                m_filePath = "";
                m_shaderType = ShaderType::UNKNOWN;
                m_entryPointName = "main";
        }

} // namespace gfxp

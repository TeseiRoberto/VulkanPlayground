
/**
 * @file shaderDescription.hpp
 * Declares the gfxp::ShaderDescription class
*/

#ifndef GFXP_SHADER_DESCRIPTION_H
#define GFXP_SHADER_DESCRIPTION_H

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

#include "gfxpLog.hpp"
#include "gfxpEnums.hpp"
#include "gfxpHandleTypes.hpp"

namespace gfxp {


        /**
         * @class ShaderDescription 
         * Class used to describe the properties of a programmable stage of graphics pipeline (shader) GPU resource
        */
        class ShaderDescription {
        public:

                /**
                 * @struct VertexBindingDescription
                 * Describes a vertex binding point of the shader
                 * @note A vertex binding point is a slot to which a vertex buffer can be bound
                 * @note Used only for descriptions related to vertex shaders
                */
                struct VertexBindingDescription {
                        uint32_t                slotIndex = 0;                                  ///< Numeric index of the binding point
                        VertexInputRate         inputRate = VertexInputRate::PER_VERTEX;        ///< Rate at which data will be fetched from the vertex buffer
                        uint32_t                stride = 0;                                     ///< Bytes between two different "data entries" stored in the vertex buffer
                };


                /**
                 * @struct VertexAttributeDescription
                 * Describes a vertex attribute taken as input by the shader
                 * @note Used only for descriptions related to vertex shaders
                */
                struct VertexAttributeDescription {
                        uint32_t                bindingSlotIndex = 0;                           ///< Vertex binding point (slot) from which the attribute is fetched
                        uint32_t                location = 0;                                   ///< Unique numeric id used to identify the attribute (must be unique even if different binding points are used)
                        VertexAttributeType     dataType = VertexAttributeType::UNKNOWN;        ///< Data type of the vertex attribute
                        size_t                  offset = 0;                                     ///< Offset of the attribute from the start of the "data entry" fetched from the vertex buffer
                };


                                                        ShaderDescription();
                                                        ~ShaderDescription() = default;

                ShaderDescription&                      setSourceFile(const std::filesystem::path& filePath, bool isBinary, ShaderType type);
                ShaderDescription&                      setEntryPointFunctionName(const std::string& name);

                ShaderDescription&                      describeVertexBinding(uint32_t slotIndex, VertexInputRate inputRate, size_t stride);
                ShaderDescription&                      describeVertexAttribute(uint32_t bindingSlotIndex, uint32_t location, VertexAttributeType attributeType, size_t offset);

                inline const std::filesystem::path&     getSourceFile() const                           { return m_filePath; }
                inline bool                             isBinary() const                                { return m_isBinary; }
                inline ShaderType                       getType() const                                 { return m_shaderType; }
                inline const std::string&               getEntryPointFunctionName() const               { return m_entryPointName; }

                inline const std::vector<VertexBindingDescription>&     getVertexBindingsDesc() const   { return m_vertexBindingsDesc; }
                inline const std::vector<VertexAttributeDescription>&   getVertexAttributesDesc() const { return m_vertexAttributesDesc; }

                void                                    reset();

        private:

                bool                                    m_isBinary = false;                             ///< Indicates if m_filePath contains precompiled byte code or plain source code that must be compiled
                std::filesystem::path                   m_filePath = "";                                ///< Path to the file that contains the shader source code/precompiled byte code
                ShaderType                              m_shaderType = ShaderType::UNKNOWN_SHADER;      ///< Type of shader that shall be created from this description
                std::string                             m_entryPointName = "main";                      ///< Name of the function that shall be invoked as main when the shader is executed

                std::vector<VertexBindingDescription>   m_vertexBindingsDesc;                           ///< Structs wich describes the vertex binding points used by the shader (used only if the description is related to a vertex shader)
                std::vector<VertexAttributeDescription> m_vertexAttributesDesc;                         ///< Structs wich describes the vertex attributes used by the shader (used only if the description is related to a vertex shader)
        };


} // namespace gfxp

#endif // GFXP_SHADER_DESCRIPTION_H

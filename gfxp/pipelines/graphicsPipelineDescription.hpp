
/**
 * @file graphicsPipelineDescription.hpp
 * Declares the gfxp::GraphicsPipelineDescription class
*/

#ifndef GFXP_GRAPHICS_PIPELINE_BUILDER_H
#define GFXP_GRAPHICS_PIPELINE_BUILDER_H

#include <cstdint>
#include <map>

#include "gfxpLog.hpp"
#include "gfxpEnums.hpp"
#include "gfxpHandleTypes.hpp"
#include "shader/shaderDescription.hpp"

namespace gfxp {


        /**
         * @brief GraphicsPipelineDescription
         * Class used to describe the properties of a graphics pipeline GPU resource
        */
        class GraphicsPipelineDescription {
        public:
                                                GraphicsPipelineDescription();
                                                ~GraphicsPipelineDescription() = default;

                GraphicsPipelineDescription&    setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
                GraphicsPipelineDescription&    setInputAssembly(PrimitiveTopologyType primitiveType);

                GraphicsPipelineDescription&    setVertexShader(const ShaderHandle handle);
                GraphicsPipelineDescription&    setFragmentShader(const ShaderHandle handle);

                GraphicsPipelineDescription&    setRasterizer(bool wireframe, TriangleFrontFace frontFace, CullMode cullMode);
                //GraphicsPipelineDescription&    setMultisampling(); TODO: Add implementation...
                //GraphicsPipelineDescription&    setColorBlending(); TODO: Add implementation...

                GraphicsPipelineDescription&    setDepthTest(bool enabled, CompareOperatorType compareOp);
                //GraphicsPipelineDescription&    setStencilTest(); TODO: Add implementation...
                
                void                            reset();

        private:

                /**
                 * @struct ViewportDescription
                 * Describes the properties of a viewport for the graphics pipeline
                */
                struct ViewportDescription {
                        uint32_t x = 0;                 /// X cooridinate of the viewport rectangle's top left corner
                        uint32_t y = 0;                 /// Y cooridinate of the viewport rectangle's top left corner
                        uint32_t width = 0;             /// Width of the viewport rectangle
                        uint32_t height = 0;            /// Height of the viewport rectangle
                };


                /**
                 * @struct RasterizerStageDescription
                 * Describes the properties of the rasterization stage for the graphics pipeline
                */
                struct RasterizerStageDescription {
                        bool                    wireframe = false;                                              ///< Indicates if the graphic shapes rendered by the pipeline shall be wireframe or filled
                        TriangleFrontFace       triangleFrontFace = TriangleFrontFace::FRONT_FACE_CLOCKWISE;    ///< Indicates wich face of a triangle shall be considered the visible/front face
                        CullMode                cullMode = CullMode::NO_CULLING;                                ///< Indicates which triangle shall be culled by the graphics pipeline
                };


                /**
                 * @struct DepthTestDescription
                 * Describes if and how depth test shall be performed
                */
                struct DepthTestDescription {
                        bool                    enabled = false;                                                ///< Indicates if the depth test is enabled for the graphics pipeline
                        CompareOperatorType     compareOp = CompareOperatorType::LESS_THEN;                     ///< Type of compare operator that shall be used during depth test
                };


                bool                                                    m_isViewportSet = false;                                ///< Indicates if the setViewport method has been called or not
                ViewportDescription                                     m_viewportDesc = {};                                    ///< Struct that describes the viewport
                PrimitiveTopologyType                                   m_primitiveType = PrimitiveTopologyType::TRIANGLE_LIST; ///< Type of primitive graphic shapes that the pipeline shall render
                std::map<ShaderType, ShaderHandle>                      m_shadersDesc;                                          ///< Indicates shaders to be used for the programmable stages of the graphics pipeline
                RasterizerStageDescription                              m_rasterizerDesc = {};                                  ///< Struct that describes properties for the fixed rasterization stage of the graphics pipeline
                DepthTestDescription                                    m_depthTestDesc = {};                                   ///< Struct that describes if and how depth test shall be performed by the graphics pipeline
        };


} // namespace gfxp

#endif // GFXP_GRAPHICS_PIPELINE_BUILDER_H


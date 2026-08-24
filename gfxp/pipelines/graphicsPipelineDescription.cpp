
/**
 * @file graphicsPipelineDescription.cpp
 * Implements the gfxp::GraphicsPipelineDescription class
*/

#include "graphicsPipelineDescription.hpp"

namespace gfxp {


        /**
         * @brief GraphicsPipelineDescription::GraphicsPipelineDescription
         * Class constructor, creates the description of a graphics pipeline with default settings (see @link GraphicsPipelineDescription::reset)
        */
        GraphicsPipelineDescription::GraphicsPipelineDescription()
        {
                reset();
        }


        /**
         * @brief GraphicsPipelineDescription::setViewport
         * Sets the viewport (region of the framebuffer to which pixels produced by the pipeline, created using this
         * decsription instance, can be written)
         * @param x X coordinate of the viewport rectangle's top left corner
         * @param y Y coordinate of the viewport rectangle's top left corner
         * @param width Width of the viewport
         * @param height Height of the viewport
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineDescription& GraphicsPipelineDescription::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
                // TODO: Support multiple viewports for multiple render targets (and make scissor region configurable???)

                m_isViewportSet = true;

                m_viewportDesc.x = x;
                m_viewportDesc.y = y;
                m_viewportDesc.width = width;
                m_viewportDesc.height = height;

                return *this;
        }


        /**
         * @brief GraphicsPipelineDescription::setPrimitiveTopologyType
         * Sets the type of primitive topology that will be rendered by the pipeline created using this description instance
         * @param primitiveType Primitive topology type that will be rendered by the pipeline
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineDescription& GraphicsPipelineDescription::setPrimitiveTopologyType(PrimitiveTopologyType primitiveType)
        {
                m_primitiveType = primitiveType;
                return *this;
        }


        /**
         * @brief GraphicsPipelineDescription::setVertexShader
         * Sets the vertex shader that shall be used by the pipeline created using this description
         * @param handle Handle to the vertex shader object, such handle can be destroyed safely after graphics pipeline creation
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineDescription& GraphicsPipelineDescription::setVertexShader(const ShaderHandle handle)
        {
                if(handle == GFXP_INVALID_HANDLE)
                {
                        LOG_ERROR("GraphicsPipelineDescription::setVertexShader() failed: handle to shader object is invalid!");
                        return *this;
                }

                m_shadersDesc.insert( { ShaderType::VERTEX_SHADER, handle } );
                return *this;
        }


        /**
         * @brief GraphicsPipelineDescription::setFragmentShader
         * Sets the fragment shader that shall be used by the pipeline created using this description
         * @param handle Handle to the fragment shader object, such handle can be destroyed safely after graphics pipeline creation
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineDescription& GraphicsPipelineDescription::setFragmentShader(const ShaderHandle handle)
        {
                if(handle == GFXP_INVALID_HANDLE)
                {
                        LOG_ERROR("GraphicsPipelineDescription::setfragmentShader() failed: handle to shader object is invalid!");
                        return *this;
                }

                m_shadersDesc.insert( { ShaderType::FRAGMENT_SHADER, handle } );
                return *this;
        }


        /**
         * @brief GraphicsPipelineDescription::setRasterizer
         * Sets properties for the rasterization stage of the pipeline that will be created using this description
         * @param frontFace Defines which face of a triangle shall be considered the front face 
         * @param cullMode Triangle facing direction used for primitive culling
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineDescription& GraphicsPipelineDescription::setRasterizer(bool wireframe, TriangleFrontFace frontFace, CullMode cullMode)
        {
                m_rasterizerDesc.wireframe              = wireframe;
                m_rasterizerDesc.triangleFrontFace      = frontFace;
                m_rasterizerDesc.cullMode               = cullMode;

                return *this;
        }


        /**
         * @brief GraphicsPipelineDescription::setDepthTest
         * Enables/disables depth test for the pipeline that will be created using this description
         * @param enabled Indicates if depth test shall be enabled or not
         * @param compareOp Specifies the comparison operator to be used for the depth test
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineDescription& GraphicsPipelineDescription::setDepthTest(bool enabled, CompareOperatorType compareOp)
        {
                m_depthTestDesc.enabled         = enabled;
                m_depthTestDesc.compareOp       = compareOp;

                return *this;
        }


        /**
         * @brief GraphicsPipelineDescription::getVertexShader
         * @return handle to the shader that shall be used as vertex shader for the graphics pipeline,
         *      GFXP_INVALID_HANDLE if no vertex shader is set
        */
        const ShaderHandle GraphicsPipelineDescription::getVertexShader() const
        {
                auto it = m_shadersDesc.find(gfxp::ShaderType::VERTEX_SHADER);
                if(it == m_shadersDesc.end())
                        return GFXP_INVALID_HANDLE;

                return it->second;
        }


        /**
         * @brief GraphicsPipelineDescription::getFragmentShader
         * @return handle to the shader that shall be used as vertex shader for the graphics pipeline,
         *      GFXP_INVALID_HANDLE if no fragment shader is set
        */
        const ShaderHandle GraphicsPipelineDescription::getFragmentShader() const
        {
                auto it = m_shadersDesc.find(gfxp::ShaderType::FRAGMENT_SHADER);
                if(it == m_shadersDesc.end())
                        return GFXP_INVALID_HANDLE;

                return it->second;
        }


        /**
         * @brief GraphicsPipelineDescription::reset
         * Resets the description to the default configuration.
         * By default a GraphicsPipelineDescription instance is setup to create a graphics pipeline with:
         *      - filled triangles primitive topology
         *      - disabled depth and stencil tests
         *      - disabled color blending
         *      - clockwise triangle front face
         *      - disabled culling
         *      - disabled multisampling (sample count equal to 1)
         *
         * To create a valid graphics pipeline description the user must define at least:
         *      - a vertex shader (with its input description)
         *      - a fragment shader (with its input description)
         *      - viewport dimensions
        */
        void GraphicsPipelineDescription::reset()
        {
                // Reset viewport description
                m_isViewportSet = false;
                m_viewportDesc = {};

                // Reset primitive topology type
                m_primitiveType = PrimitiveTopologyType::TRIANGLE_LIST;

                // Reset programmable shader stages descriptions
                m_shadersDesc.clear();

                // Reset rasterizer stage description
                m_rasterizerDesc = {};

                // Reset depth test description
                m_depthTestDesc = {};
        }


} // namespace gfxp

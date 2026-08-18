
/**
 * @file iGraphicContext.hpp
 * Declares the gfxp::IGraphicContext interface
*/

#ifndef GFXP_IGRAPHIC_CONTEXT_H
#define GFXP_IGRAPHIC_CONTEXT_H

#include <cstdint>
#include <memory>

#include "gfxpEnums.hpp"
#include "gfxpHandleTypes.hpp"

namespace gfxp {


        /**
         * @class IGraphicContext
         * Interface implemented by API specfic graphic context classes
        */
        class IGraphicContext {
        public:
                virtual                                 ~IGraphicContext() = default;
                static std::unique_ptr<IGraphicContext> create(const GraphicsApi backendApi);

                // Disable copies
                IGraphicContext(const IGraphicContext& other) = delete;
                IGraphicContext(const IGraphicContext&& other) = delete;

                IGraphicContext& operator =(const IGraphicContext& other) = delete;
                IGraphicContext& operator =(const IGraphicContext&& other) = delete;

                // ======================================================================
                // Context initialization and termination
                virtual bool                    init() = 0;
                virtual void                    terminate() = 0;
                virtual bool                    isInit() = 0;

                // ======================================================================
                // GPU resources creation
                virtual BufferHandle            createBuffer(const size_t size, const BufferType type) = 0;
                virtual ImageHandle             createImage(const ImageDescription& imgDesc) = 0;
                virtual ShaderHandle            createShader(const ShaderDescription& shaderDesc) = 0;
                virtual PipelineHandle          createGraphicsPipeline(const GraphicsPipelineDescription& shaderDesc) = 0;

                // ======================================================================
                // GPU resources destruction
                virtual void                    destroyBuffer(const BufferHandle handle) = 0;
                virtual void                    destroyImage(const ImageHandle handle) = 0;
                virtual void                    destroyShader(const ShaderHandle handle) = 0;
                virtual void                    destroyGraphicsPipeline(const PipelineHandle handle) = 0;

        protected:
                                IGraphicContext() = default;
        };


} // namespace gfxp

#endif // GFXP_IGRAPHIC_CONTEXT_H

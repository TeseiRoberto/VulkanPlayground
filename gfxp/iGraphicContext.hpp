
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


        // Forward declarations for GPU resource description classes
        class BufferDescription;
        class TextureDescription;
        class ShaderDescription;
        class GraphicsPipelineDescription;


        /**
         * @class IGraphicContext
         * Interface implemented by API specfic graphic context classes
        */
        class IGraphicContext {
        public:
                virtual                                 ~IGraphicContext() = default;
                static std::unique_ptr<IGraphicContext> create(const GraphicApi backendApi);

                                                        // Disable copies
                                                        IGraphicContext(const IGraphicContext& other) = delete;
                                                        IGraphicContext(const IGraphicContext&& other) = delete;

                                                        IGraphicContext& operator =(const IGraphicContext& other) = delete;
                                                        IGraphicContext& operator =(const IGraphicContext&& other) = delete;

                // ====================================================================================================
                // Context initialization and termination
                virtual bool                            init() = 0;
                virtual void                            terminate() = 0;
                virtual bool                            isInit() const = 0;

                // ====================================================================================================
                // GPU resources creation
                virtual BufferHandle                    createBuffer(const size_t size, const BufferUsageFlags usage) = 0;
                virtual TextureHandle                   createTexture(const TextureDescription& textureDesc) = 0;
                virtual ShaderHandle                    createShader(const ShaderDescription& shaderDesc) = 0;
                virtual PipelineHandle                  createGraphicsPipeline(const GraphicsPipelineDescription& shaderDesc) = 0;

                // ====================================================================================================
                // GPU resources destruction
                virtual void                            destroyBuffer(BufferHandle& handle) = 0;
                virtual void                            destroyTexture(TextureHandle& handle) = 0;
                virtual void                            destroyShader(ShaderHandle& handle) = 0;
                virtual void                            destroyGraphicsPipeline(PipelineHandle& handle) = 0;

        protected:
                                                        IGraphicContext() = default;
        };


} // namespace gfxp

#endif // GFXP_IGRAPHIC_CONTEXT_H

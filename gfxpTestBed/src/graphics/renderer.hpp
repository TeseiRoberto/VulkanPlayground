
/**
 * @file renderer.hpp
 * Declares the Renderer class
*/

#ifndef RENDERER_H
#define _RENDERER_H

#include <cstdint>
#include <vector>
#include <GLFW/glfw3.h>

#include "gfxp/gfxpLog.hpp"
#include "gfxp/gfxpEnums.hpp"
#include "gfxp/gfxpHandleTypes.hpp"
#include "gfxp/iGraphicContext.hpp"
#include "gfxp/shaders/shaderDescription.hpp"
#include "gfxp/pipelines/graphicsPipelineDescription.hpp"


class Renderer {
public:
        explicit        Renderer(std::unique_ptr<gfxp::IGraphicContext>& context);
                        ~Renderer();

        bool            init(GLFWwindow* wnd);
        void            terminate();
        bool            isInit() const          { return m_isInit; }

        void            drawFrame();

private:

        bool            createGraphicsPipeline();
        void            destroyGraphicsPipeline();

        bool            createStagingBuffer();
        void            destroyStagingBuffer();


        bool                                            m_isInit = false;                               ///< Indicates if the renderer has been initialized correctly

        std::unique_ptr<gfxp::IGraphicContext>&         m_context;                                      ///< Graphic context instance used by the renderer
        gfxp::PipelineHandle                            m_gfxPipeline = gfxp::INVALID_HANDLE;           ///< Graphics pipeline used by the renderer
        gfxp::BufferHandle                              m_stagingBuffer = gfxp::INVALID_HANDLE;         ///< Buffer used to transfer data from CPU to GPU

        // Temporary methods and data necessary to render a simple rectangle
        bool                                            createRectangleBuffers();
        void                                            destroyRectangleBuffers();
        
        gfxp::BufferHandle                              m_rectVertexBuffer = gfxp::INVALID_HANDLE;      ///< Vertex buffer used to store rectangle's vertices data
        gfxp::BufferHandle                              m_rectIndexBuffer = gfxp::INVALID_HANDLE;       ///< Index buffer used to store rextangle's indices data
        // ==================================================
};


#endif // RENDERER_H

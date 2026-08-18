
/**
 * @file renderer.cpp
 * Implements the Renderer class
*/

#include "renderer.hpp"


/**
 * @brief Renderer::Renderer
 * Renderer constructor
 * @param context Graphic context instance that will be used by the renderer
*/
Renderer::Renderer(std::unique_ptr<gfxp::IGraphicContext>& context)
        : m_context(context)
{
        // Nothing to do here...
}


/**
 * @brief Renderer::~Renderer
 * Renderer destructor, calls terminate if not called yet
*/
Renderer::~Renderer()
{
        if(isInit())
                terminate();
}


/**
 * @brief Renderer::init
 * Initializes all the resources needed by the renderer
 * @param wnd Window in which the renderer will draw
 * @return true on success, false on failure
 * @note GLFW must be initialized before this method is called!
*/
bool Renderer::init(GLFWwindow* wnd)
{
        #define CHECK(x) if( !x ) { terminate(); return false; }

        // Ensure graphic context validity
        if( m_context == nullptr || !m_context->isInit() )
                return false;

        CHECK( createGraphicsPipeline() )
        CHECK( createStagingBuffer() )
        CHECK( createRectangleBuffers() )
        return true;
}


/**
 * @brief Renderer::terminate
 * Terminates all the resources initialized by the renderer
*/
void Renderer::terminate()
{
        if( !isInit() )
                return;

        destroyGraphicsPipeline();
        destroyStagingBuffer();
        destroyRectangleBuffers();
}


/*!
 * @brief Renderer::drawFrame
 * Draws a single frame
*/
void Renderer::drawFrame()
{
        // TODO: Add implementation...
}


/**
 * @brief Renderer::createGraphicsPipeline
 * Creates the graphics pipeline that will be used by the renderer.
 * @return True on success, false on failure
*/
bool Renderer::createGraphicsPipeline()
{
        if(m_gfxPipeline != GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createGraphicsPipeline(): graphics pipeline has already been created!");
                return true;
        }

        m_gfxPipeline = m_context->createGraphicsPipeline();
        if(m_gfxPipeline == GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createGraphicsPipeline() failed: graphics pipeline creation failed!");
                return false;
        }

        return true;
}


/**
 * @brief Renderer::destroyGraphicsPipeline
 * Destroys the graphics pipeline used by the renderer
*/
void Renderer::destroyGraphicsPipeline()
{
        m_context->destroyGraphicsPipeline(m_gfxPipeline);
}


/**
 * @brief Renderer::createStagingBuffer
 * Creates the staging buffer that will be used by the renderer
 * @return True on success, false on failure
*/
bool Renderer::createStagingBuffer()
{
        // Check if the buffer has already been created
        if(m_stagingBuffer != GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createStagingBuffer(): staging buffer has already been created!");
                return true;
        }

        m_stagingBuffer = m_context->createBuffer(1024, gfxp::BufferType::STAGING_BUFFER);
        if(m_stagingBuffer == GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createStagingBuffer() failed: staging buffer creation failed!");
                return false;
        }

        return true;
}


/**
 * @brief Renderer::destroyStagingBuffer
 * Destroys the staging buffer used by the renderer
*/
void Renderer::destroyStagingBuffer()
{
        m_context->destroyBuffer(m_stagingBuffer);
}


/**
 * @brief Renderer::createRectangleBuffers
 * Temporary method used to create and populate the vertex and index buffers necessary
 * to render a colored rectangle
 * @return True on success, false on failure
*/
bool Renderer::createRectangleBuffers()
{
        // Check if buffer have already been created
        if(m_rectVertexBuffer != GFXP_INVALID_HANDLE || m_rectIndexBuffer != GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createRectangleBuffers(): buffers have already been created!");
                return true;
        }

        const float vertexData[] = {
                // position             color
                -0.5f, 0.5f, 0.0f,      1.0f, 1.0f, 1.0f,       // Bottom left
                -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,       // Top left
                0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,       // Top right
                0.5f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f,       // Bottom right
        };

        const uint16_t indexData[] = {
                0, 1, 2,                // Top left triangle
                2, 3, 0                 // Bottom right triangle
        };

        // Try to create buffers
        m_rectVertexBuffer = m_context->createBuffer(sizeof(vertexData), gfxp::BufferType::VERTEX_BUFFER);
        m_rectIndexBuffer = m_context->createBuffer(sizeof(indexData), gfxp::BufferType::INDEX_BUFFER);

        if(m_rectVertexBuffer == GFXP_INVALID_HANDLE || m_rectIndexBuffer == GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createRectangleBuffers() failed: vertex and/or index buffer creation failed!");
                return false;
        }

        // TODO: Transfer data to the created buffer using m_stagingBuffer

        return true;
}


/**
 * @brief Renderer::destroyRectangleBuffers
 * Temporary method used to destroy the vertex and index buffers used to render a colored rectangle
*/
void Renderer::destroyRectangleBuffers()
{
        m_context->destroyBuffer(m_rectVertexBuffer);
        m_context->destroyBuffer(m_rectIndexBuffer);
}


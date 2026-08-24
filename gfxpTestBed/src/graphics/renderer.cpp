
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
        if(m_gfxPipeline != gfxp::GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createGraphicsPipeline(): graphics pipeline has already been created!");
                return true;
        }

        // Describe the vertex and the fragment shader
        gfxp::ShaderDescription vrtxShaderDesc;
        gfxp::ShaderDescription fragShaderDesc;

        vrtxShaderDesc
                .setSourceFile("../../gfxpTestBed/resources/shaders/bin/vertexShader.spv", true, gfxp::ShaderType::VERTEX_SHADER)
                .describeVertexBinding(0, gfxp::VertexInputRate::PER_VERTEX, sizeof(float) * 6)                 // We only use 1 vertex buffer
                .describeVertexAttribute(0, 0, gfxp::VertexAttributeType::VEC3_FLOAT, 0)                        // Position vertex attribute
                .describeVertexAttribute(0, 1, gfxp::VertexAttributeType::VEC3_FLOAT, sizeof(float) * 3);       // Color vertex attribute

        fragShaderDesc
                .setSourceFile("../../gfxpTestBed/resources/shaders/bin/fragmentShader.spv", true, gfxp::ShaderType::FRAGMENT_SHADER);

        // Create shader objects
        gfxp::ShaderHandle vrtxShader = m_context->createShader(vrtxShaderDesc);
        gfxp::ShaderHandle fragShader = m_context->createShader(fragShaderDesc);

        if(vrtxShader == gfxp::GFXP_INVALID_HANDLE || fragShader == gfxp::GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createGraphicsPipeline() failed: creation of vertex and/or fragment shader failed!");
                m_context->destroyShader(vrtxShader);
                m_context->destroyShader(fragShader);

                return false;
        }

        // Describe the graphics pipeline
        gfxp::GraphicsPipelineDescription pipelineDesc;

        pipelineDesc
                .setViewport(0, 0, 1024, 1024)                  // TODO: Set viewport size using swapchain extent!
                .setVertexShader(vrtxShader)
                .setRasterizer(false, gfxp::TriangleFrontFace::FRONT_FACE_CLOCKWISE, gfxp::CullMode::NO_CULLING)
                .setFragmentShader(fragShader);

        m_gfxPipeline = m_context->createGraphicsPipeline(pipelineDesc);

        if(m_gfxPipeline == gfxp::GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createGraphicsPipeline() failed: graphics pipeline creation failed!");
                m_context->destroyShader(vrtxShader);
                m_context->destroyShader(fragShader);

                return false;
        }

        m_context->destroyShader(vrtxShader);
        m_context->destroyShader(fragShader);
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
        if(m_stagingBuffer != gfxp::GFXP_INVALID_HANDLE)
        {
                LOG_WARN("Renderer::createStagingBuffer(): staging buffer has already been created!");
                return true;
        }

        m_stagingBuffer = m_context->createBuffer(1024, gfxp::BufferType::STAGING_BUFFER);
        if(m_stagingBuffer == gfxp::GFXP_INVALID_HANDLE)
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
        if(m_rectVertexBuffer != gfxp::GFXP_INVALID_HANDLE || m_rectIndexBuffer != gfxp::GFXP_INVALID_HANDLE)
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

        if(m_rectVertexBuffer == gfxp::GFXP_INVALID_HANDLE || m_rectIndexBuffer == gfxp::GFXP_INVALID_HANDLE)
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


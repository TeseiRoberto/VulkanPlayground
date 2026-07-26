
/**
 * @file renderer.hpp
 * Defines the vp::Renderer class
*/

#ifndef VP_RENDERER_H
#define VP_RENDERER_H

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vulkanContext.hpp"
#include "vulkanShader.hpp"
#include "log.hpp"

namespace vp {


        class Renderer {
        public:
                explicit        Renderer();
                                ~Renderer();

                bool            init(GLFWwindow* wnd);
                void            terminate();

                inline bool     isInit() const          { return m_context.isInit(); }

                void            drawFrame();

        private:

                /**
                 * @struct SwapchainProps
                 * Utility struct used to keep track of properties of the
                 * Swapchain used by the renderer
                */
                struct SwapchainProps {
                        VkFormat                imageFormat;                    ///< Format used by the swapchain images
                        VkColorSpaceKHR         colorSpace;                     ///< Color space used by the swapchain images
                        VkExtent2D              extent = { 0, 0 };              ///< Size of the swapchain images
                };


                /**
                 * @struct Image
                 * Keeps track of the handle to a vulkan image object and the allocated memory for the image data.
                 *
                 * @note The VkImage object defines the logical properties of a texture (texels format,
                 *      texture size, mipmap levels), such object is used to specify how the GPU should
                 *      interpret image data.
                 *      The actual memory buffer in which image data gets stored must be allocated
                 *      explicitly on the GPU, and then bound to the image object.
                */
                struct Image {
                        VkImage                 handle = VK_NULL_HANDLE;        ///< Handle to the image object
                        VkDeviceMemory          memory = VK_NULL_HANDLE;        ///< Handle to the memory buffer allocated for the image data
                };


                /**
                 * @struct Buffer
                 * Keeps track of the handle to a vulkan buffer object and the GPU memory allocated for it.
                 *
                 * @note The VkBuffer object defines the logical properties of a buffer (GPU memory area).
                 *      The actual memory area in which data gets stored must be allocated
                 *      explicitly on the GPU, and then bound to the buffer object.
                */
                struct Buffer {
                        VkBuffer                handle = VK_NULL_HANDLE;        ///< Handle to the buffer object
                        VkDeviceMemory          memory = VK_NULL_HANDLE;        ///< Handle to the memory area allocated for the buffer
                        VkDeviceSize            size = 0;                       ///< Size of the data currently stored into the buffer, expressed in bytes
                        VkDeviceSize            capacity = 0;                   ///< Capacity of the buffer, expressed in bytes
                };


                /**
                 * @struct StagingBuffer
                 * Models a buffer object that is used as temporary storage to upload data from CPU to GPU buffers.
                 * Such buffer object is associated to a memory area which is host visible (accessible by both CPU and GPU) and host coherent
                */
                struct StagingBuffer : Buffer {
                        void*                   rawPtr = nullptr;               ///< Pointer to the mapped memory area allocated for the staging buffer
                };


                bool            createSurface(GLFWwindow* wnd);
                void            destroySurface();

                bool            createSwapchain(GLFWwindow* wnd);
                void            destroySwapchain();

                bool            createDepthAttachment();
                void            destroyDepthAttachment();

                bool            createRenderPass();
                void            destroyRenderPass();

                bool            createGraphicsPipeline();
                void            destroyGraphicsPipeline();

                bool            createFramebuffers();
                void            destroyFramebuffers();

                bool            createCommandPools();
                void            destroyCommandPools();

                bool            createCommandBuffers();
                bool            recordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t imageIndex);

                bool            createSynchObjects();
                void            destroySynchObjects();


                // Methods for resources management
                bool            createBuffer(Buffer& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags);
                void            destroyBuffer(Buffer& buffer);

                bool            createStagingBuffer(StagingBuffer& buffer, VkDeviceSize size);
                void            destroyStagingBuffer(StagingBuffer& buffer);

                bool            copyBuffer(Buffer& from, Buffer& to);

                bool            createImage(Image& image, uint32_t width, uint32_t height, uint32_t depth, VkImageType type, uint32_t mipLevels,
                                                VkFormat format, VkImageTiling tilingMode, VkImageUsageFlags usageFlags);

                void            destroyImage(Image& image);
                // ==================================================

                // Temporary methods used to render a simple triangle
                bool            createTriangleVertexBuffer();
                void            destroyTriangleVertexBuffer();
                // ==================================================

                uint32_t        findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);


                static constexpr uint32_t       MAX_FRAMES_IN_FLIGHT = 2;                               ///< Max number of frames that can be rendered asinchronously by the renderer
                uint32_t                        m_currFrameNum = 0;                                     ///< Number/index of the current frame being rendered

                VulkanContext                   m_context;                                              ///< Vulkan context instance used by the renderer

                VkSurfaceKHR                    m_surface = VK_NULL_HANDLE;                             ///< Surface on which images produced by the renderer will be presented
                
                SwapchainProps                  m_swapchainProps;                                       ///< Properties of the swapchain used by the renderer
                VkSwapchainKHR                  m_swapchain = VK_NULL_HANDLE;                           ///< Swapchain that owns images on which renderer will draw
                std::vector<VkImage>            m_swapchainImages;                                      ///< Handles to images owned by the swapchain that the renderer is using
                std::vector<VkImageView>        m_swapchainImagesViews;                                 ///< Handles to views of images owned by the swapchain 

                VkFormat                        m_depthAttachmentFormat = VK_FORMAT_UNDEFINED;          ///< Image format used for the depth image attachment
                Image                           m_depthAttachment;                                      ///< Image used to implement depth test
                VkImageView                     m_depthAttachmentView = VK_NULL_HANDLE;                 ///< View used for the deph attachment image

                VkRenderPass                    m_renderPass = VK_NULL_HANDLE;                          ///< The main render pass used by the renderer
                VkPipelineLayout                m_pipelineLayout = VK_NULL_HANDLE;                      ///< Describes the resources used by the renderer's graphics pipeline
                VkPipeline                      m_pipeline = VK_NULL_HANDLE;                            ///< Handle to the graphics pipeline used by the renderer to draw graphics

                std::vector<VkFramebuffer>      m_swapchainFramebuffers;                                ///< Framebuffer objects used to bind swapchain images before rendering operations

                VkCommandPool                   m_gfxCommandPool = VK_NULL_HANDLE;                      ///< Pool that manages command buffers used for draw operations 
                VkCommandPool                   m_transferCommandPool = VK_NULL_HANDLE;                 ///< Pool that manages command buffers used for data transfer operations
                VkCommandBuffer                 m_gfxCmdBuffers[MAX_FRAMES_IN_FLIGHT];                  ///< Command buffers that will be sent by the renderer to the graphics queue
        
                VkSemaphore                     m_imageAvailableSemaphores [MAX_FRAMES_IN_FLIGHT];      ///< Semaphores used to signal that an image has been acquired from the swapchain (we are ready to render on it)
                VkSemaphore                     m_renderingFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];    ///< Semaphores used to signal that rendering is done and one of the images can be presented
                VkFence                         m_renderingFinishedFences[MAX_FRAMES_IN_FLIGHT];        ///< Fences used to signal that rendering is done, a command buffer can be reused

                // Temporary data used to render a simple triangle
                StagingBuffer                   m_stagingBuffer = {};                                   ///< Buffer used to transfer data from CPU to GPU
                Buffer                          m_triangleVertexBuffer = {};                            ///< Vertex buffer used to store triangle's data
                // ==================================================
        };

}

#endif // VP_RENDERER_H

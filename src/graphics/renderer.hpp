
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
                explicit        Renderer() = default;
                                ~Renderer();

                bool            init(GLFWwindow* wnd);
                void            terminate();

                inline bool     isInit() const          { return m_context.isInit(); }

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
                 * Keeps track of the handle to a vulkan image object and the
                 * allocated memory for the image data.
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

                bool            createImage(Image& image, uint32_t width, uint32_t height, uint32_t depth, VkImageType type, uint32_t mipLevels,
                                                VkFormat format, VkImageTiling tilingMode, VkImageUsageFlags usageFlags);

                void            destroyImage(Image& image);

                uint32_t        findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);


                VulkanContext                   m_context;                                      ///< Vulkan context instance used by the renderer

                VkSurfaceKHR                    m_surface = VK_NULL_HANDLE;                     ///< Surface on which images produced by the renderer will be presented
                
                SwapchainProps                  m_swapchainProps;                               ///< Properties of the swapchain used by the renderer
                VkSwapchainKHR                  m_swapchain = VK_NULL_HANDLE;                   ///< Swapchain that owns images on which renderer will draw
                std::vector<VkImage>            m_swapchainImages;                              ///< Handles to images owned by the swapchain that the renderer is using
                std::vector<VkImageView>        m_swapchainImagesViews;                         ///< Handles to views of images owned by the swapchain 

                VkFormat                        m_depthAttachmentFormat = VK_FORMAT_UNDEFINED;  ///< Image format used for the depth image attachment
                Image                           m_depthAttachment;                              ///< Image used to implement depth test
                VkImageView                     m_depthAttachmentView = VK_NULL_HANDLE;         ///< View used for the deph attachment image
        
                VkRenderPass                    m_renderPass = VK_NULL_HANDLE;                  ///< The main render pass used by the renderer
                VkPipelineLayout                m_pipelineLayout = VK_NULL_HANDLE;              ///< Describes the resources used by the renderer's graphics pipeline
                VkPipeline                      m_pipeline = VK_NULL_HANDLE;                    ///< Handle to the graphics pipeline used by the renderer to draw graphics
        
                std::vector<VkFramebuffer>      m_swapchainFramebuffers;                        ///< Framebuffer objects used to bind swapchain images before rendering operations
        };

}

#endif // VP_RENDERER_H

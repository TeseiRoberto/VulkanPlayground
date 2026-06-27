
/**
 * @file renderer.cpp
 * Implements the vp::Renderer class
*/

#include "renderer.hpp"

namespace vp {


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
         * Initializes the renderer and its vulkan context
         * @param wnd Window in which the renderer will draw
         * @return true on success, false on failure
         * @note GLFW must be initialized before this method is called!
        */
        bool Renderer::init(GLFWwindow* wnd)
        {
                #define CHECK(x) if( !x ) { terminate(); return false; }

                if( !m_context.init() )
                        return false;

                CHECK( createSurface(wnd) )
                CHECK( createSwapchain(wnd) )
                CHECK( createDepthAttachment() )
                CHECK( createGraphicsPipeline() )

                // TODO: Add other stuff...
        
                return true;
        }


        /**
         * @brief Renderer::terminate
         * Terminates the renderer and its vulkan context
        */
        void Renderer::terminate()
        {
                // TODO: Add other stuff...

                destroyGraphicsPipeline();
                destroyDepthAttachment();
                destroySwapchain();
                destroySurface();

                m_context.terminate();
        }




        /**
         * @brief Renderer::createSurface
         * Creates the surface on which images produced by the renderer will be presented
         * @param wnd Window for which the surface shall be created
         * @return True on success, false on failure
         * @note The surface is a vulkan abstraction for the platform specific window, and
         * it is the target to which images to be displayed are sent
        */
        bool Renderer::createSurface(GLFWwindow* wnd)
        {
                if(wnd == nullptr)
                {
                        LOG_ERROR("Renderer::init() failed: cannot create surface, given window is nullptr!");
                        return false;
                }

                if( glfwCreateWindowSurface(m_context.getInstance(), wnd, nullptr, &m_surface) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::init() failed: cannot create surface, glfwCreateWindowSurface() failed!");
                        return false;
                }

                return true;
        }


        /**
         * @brief Renderer::destroySurface
         * Destroys the surface on which images produced by the renderer are presented
        */
        void Renderer::destroySurface()
        {
                if(m_surface == VK_NULL_HANDLE)
                        return;
        
                vkDestroySurfaceKHR(m_context.getInstance(), m_surface, nullptr);
                m_surface = VK_NULL_HANDLE;
        }


        /**
         * @brief Renderer::createSwapchain
         * Creates the swapchain used by the renderer and acquires handles to its images
         * @param wnd Window for which the swapchain shall be created
         * @return True on success, false on failure
         * @note The swapchain is simply an array of images, the renderer picks one of those images
         * and draws on it, then sends it to the surface to be displayed; after that the renderer
         * repeats the process but on another image of the array
        */
        bool Renderer::createSwapchain(GLFWwindow* wnd)
        {
                if(wnd == nullptr)
                {
                        LOG_ERROR("Renderer::init() failed: cannot create swapchain, given window is nullptr!");
                        return false;
                }

                // Try to retrieve capabilities of the surface
                VkSurfaceCapabilitiesKHR surfaceCaps {};
                if( vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_context.getPhysicalDevice(), m_surface, &surfaceCaps) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::init() failed: cannot create swapchain, vkGetPhysicalDeviceSurfaceCapabilitiesKHR() failed!");
                        return false;
                }

                // Determine dimensions of swapchain images
                m_swapchainProps.extent = surfaceCaps.currentExtent;
                if(surfaceCaps.currentExtent.width = 0xFFFFFFFF)
                {
                        int width = 0;
                        int height = 0;

                        glfwGetFramebufferSize(wnd, &width, &height);

                        m_swapchainProps.extent.width = static_cast<uint32_t>(width);
                        m_swapchainProps.extent.height = static_cast<uint32_t>(height);
                }

                m_swapchainProps.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
                const VkColorSpaceKHR imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

                VkSwapchainCreateInfoKHR swapchainInfo {};
                swapchainInfo.sType             = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                swapchainInfo.surface           = m_surface;
                swapchainInfo.minImageCount     = surfaceCaps.minImageCount;
                swapchainInfo.imageFormat       = m_swapchainProps.imageFormat;
                swapchainInfo.imageColorSpace   = m_swapchainProps.colorSpace;
                swapchainInfo.imageExtent       = m_swapchainProps.extent;
                swapchainInfo.imageArrayLayers  = 1;
                swapchainInfo.imageUsage        = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                swapchainInfo.preTransform      = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
                swapchainInfo.compositeAlpha    = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                swapchainInfo.presentMode       = VK_PRESENT_MODE_FIFO_KHR;

                if( vkCreateSwapchainKHR(m_context.getLogicalDevice(), &swapchainInfo, nullptr, &m_swapchain) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::init() failed: cannot create swapchain, vkCreateSwapchainKHR() failed!");
                        return false;
                }

                // Aqquire handles to swapchain images
                uint32_t imageCount = 0;
                if( vkGetSwapchainImagesKHR(m_context.getLogicalDevice(), m_swapchain, &imageCount, nullptr) != VK_SUCCESS)
                {
                        LOG_ERROR("Renderer::init() failed: cannot acquire swapchain images, vkGetSwapchainImages() failed!");
                        return false;
                }

                m_swapchainImages.resize(imageCount);
                if( vkGetSwapchainImagesKHR(m_context.getLogicalDevice(), m_swapchain, &imageCount, m_swapchainImages.data()) != VK_SUCCESS)
                {
                        LOG_ERROR("Renderer::init() failed: cannot acquire swapchain images, vkGetSwapchainImages() failed!");
                        return false;
                }

                return true;
        }


        /**
         * @brief Renderer::destroySwapchain
         * Destroys the swapchain used by the renderer
        */
        void Renderer::destroySwapchain()
        {
                if(m_swapchain == VK_NULL_HANDLE)
                        return;

                m_swapchainProps = {};
                m_swapchainImages.clear();
                vkDestroySwapchainKHR(m_context.getLogicalDevice(), m_swapchain, nullptr);

                m_swapchain = VK_NULL_HANDLE;
        }


        /**
         * @brief Renderer::createDepthAttachment
         * Creates resources necessary for depth test
         * @return True on success, false on failure
        */
        bool Renderer::createDepthAttachment()
        {
                // Depth image formats required by the renderer, from the most favourite to the last one
                std::vector<VkFormat> candidates {
                        VK_FORMAT_D32_SFLOAT_S8_UINT,
                        VK_FORMAT_D24_UNORM_S8_UINT
                };

                VkFormat format = VK_FORMAT_UNDEFINED;

                // Search a format that supports depth and stencil test too
                for(VkFormat fmt : candidates)
                {
                        VkFormatProperties2 fmtProps {};
                        fmtProps.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

                        vkGetPhysicalDeviceFormatProperties2(m_context.getPhysicalDevice(), fmt, &fmtProps);

                        if(fmtProps.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                        {
                                format = fmt;
                                break;
                        }
                }

                if(format == VK_FORMAT_UNDEFINED)
                {
                        LOG_ERROR("renderer::init() failed: createDepthAttachment() failed, cannot find a valid format to create the depth attachment!");
                        return false;
                }

                // Create image for the depth attachment
                if( !createImage(m_depthAttachment, m_swapchainProps.extent.width, m_swapchainProps.extent.height,
                                        VK_IMAGE_TYPE_2D, 1, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) )
                {
                        LOG_ERROR("renderer::init() failed: createDepthAttachment() failed, creation of depth attachment image failed!");
                        return false;
                }

                // Create view into the depth attachment image
                VkImageViewCreateInfo imageViewInfo {};
                imageViewInfo.sType     = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imageViewInfo.image     = m_depthAttachment.handle;
                imageViewInfo.viewType  = VK_IMAGE_VIEW_TYPE_2D;
                imageViewInfo.format    = format;

                imageViewInfo.subresourceRange.aspectMask       = VK_IMAGE_ASPECT_DEPTH_BIT;
                imageViewInfo.subresourceRange.levelCount       = 1;
                imageViewInfo.subresourceRange.layerCount       = 1;
        
                if( vkCreateImageView(m_context.getLogicalDevice(), &imageViewInfo, nullptr, &m_depthAttachmentView) != VK_SUCCESS )
                {
                        LOG_ERROR("renderer::init() failed: createDepthAttachment() failed, creation of depth attachment image failed!");
                        return false;
                }

                return true;
        }


        /**
         * @brief Renderer::destroyDepthAttachment
         * Destroyes resources used for depth test
        */
        void Renderer::destroyDepthAttachment()
        {
                // Destroy view into the depth attachment image
                if(m_depthAttachmentView != VK_NULL_HANDLE)
                {
                        vkDestroyImageView(m_context.getLogicalDevice(), m_depthAttachmentView, nullptr);
                        m_depthAttachmentView = VK_NULL_HANDLE;
                }

                destroyImage(m_depthAttachment);
        }



        /**
         * @brief Renderer::createImage
         * Creates a VkImage with the given parameteres and allocates memory for it
         * @param image Output variable in which data for the created image will be stored
         * @param width Width of the image to be crated, expressed in pixels
         * @param height Height of the image to be crated, expressed in pixels
         * @param type Type of image to be created
         * @param mipLevels number of mip-map levels 
         * @param format Format of the texels that will compose the image to be created
         * @param tilingMode Specifies how the texels of the image should be arranged in memory
         * @param usageFlags Flags describing how the image will be used
         * @return True on success, false on failure
         *
        */
        bool Renderer::createImage(Image& image, uint32_t width, uint32_t height, VkImageType type, uint32_t mipLevels,
                        VkFormat format, VkImageTiling tilingMode, VkImageUsageFlags usageFlags)
        {
                VkImageCreateInfo imageInfo {};
                imageInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.imageType             = type;
                imageInfo.format                = format;
                imageInfo.extent.width          = width;
                imageInfo.extent.height         = height;
                imageInfo.mipLevels             = mipLevels;
                imageInfo.arrayLayers           = 1;
                imageInfo.samples               = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.tiling                = tilingMode;
                imageInfo.usage                 = usageFlags;
                imageInfo.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

                // Create the vulkan image object
                if( vkCreateImage(m_context.getLogicalDevice(), &imageInfo, nullptr, &image.handle) != VK_SUCCESS)
                {
                        LOG_ERROR("Renderer::createImage() failed: vkCreateImage() failed!");
                        return false;
                }

                // Retrieve memory requirements for the image
                VkMemoryRequirements imageMemRequirements;
                vkGetImageMemoryRequirements(m_context.getLogicalDevice(), image.handle, &imageMemRequirements);
        
                VkMemoryAllocateInfo memAllocInfo {};
                memAllocInfo.sType              = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.allocationSize     = imageMemRequirements.size;
                memAllocInfo.memoryTypeIndex    = findMemoryType(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
                // Allocate memory for the image
                if( vkAllocateMemory(m_context.getLogicalDevice(), &memAllocInfo, nullptr, &image.memory) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::createImage() failed: vkAllocateMemory() failed!");
                        destroyImage(image);
                        return false;
                }

                // Bind memory to the image
                vkBindImageMemory(m_context.getLogicalDevice(), image.handle, image.memory, 0);

                return true;
        }


        /**
         * @brief Renderer::destroyImage
         * Destroys the given image and deallocates its resources
         * @param image Image to be destroyed
        */
        void Renderer::destroyImage(Image& image)
        {
                // Destroy the image object
                if(image.handle != VK_NULL_HANDLE)
                {
                        vkDestroyImage(m_context.getLogicalDevice(), image.handle, nullptr);
                        image.handle = VK_NULL_HANDLE;
                }

                // Free memory allocated for the image
                if(image.memory != VK_NULL_HANDLE)
                {
                        vkFreeMemory(m_context.getLogicalDevice(), image.memory, nullptr);
                        image.memory = VK_NULL_HANDLE;
                }
        }


        /**
         * @brief Renderer::findMemoryType
         * Queries memory properties of the physical device used by the renderer and
         * searches the right type of GPU memory to be used according to the given parameters
         * @param typeFilter Bit field used to specify suitable memory types
         * @param props Flags used to specify the properties that shall be satisfied by the memory type
         * @return Index of the memory type (on the physical device) that shall be used,
         *      UINT32_MAX if such memory type is not found
        */
        uint32_t Renderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props)
        {
                VkPhysicalDeviceMemoryProperties memProps {};

                // Query memory properties of the physical device
                vkGetPhysicalDeviceMemoryProperties(m_context.getPhysicalDevice(), &memProps);

                for(uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
                {
                        if( typeFilter & (1 << i) && 
                                (memProps.memoryTypes[i].propertyFlags & props) == props )
                        {
                                return i;
                        }
                }

                return UINT32_MAX;
        }


        bool Renderer::createGraphicsPipeline()
        {
                // TODO: Add implementation...
                return true;
        }


        void Renderer::destroyGraphicsPipeline()
        {
                // TODO: Add implementation...
        }

}


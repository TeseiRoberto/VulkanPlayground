
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

                if( !m_context.init(true) )
                        return false;

                CHECK( createSurface(wnd) )
                CHECK( createSwapchain(wnd) )
                CHECK( createDepthAttachment() )
                CHECK( createRenderPass() )
                CHECK( createGraphicsPipeline() )
                CHECK( createFramebuffers() )

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

                destroyFramebuffers();
                destroyGraphicsPipeline();
                destroyRenderPass();
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
         * Creates the swapchain used by the renderer, acquires handles to its images
         * anc creates an image view for each of them
         * @param wnd Window for which the swapchain shall be created
         * @return True on success, false on failure
         * @note The swapchain is simply an array of images, the renderer picks one of those images
         * and draws on it, then sends it to the surface to be displayed; after that the renderer
         * repeats the process but on another image of the array.
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

                // Create image views for the images of the swapchain
                m_swapchainImagesViews.reserve( m_swapchainImages.size() );

                for(size_t i = 0; i < m_swapchainImages.size(); ++i)
                {
                        VkImageSubresourceRange subresourceView {};

                        subresourceView.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
                        subresourceView.baseMipLevel    = 0;
                        subresourceView.levelCount      = 1;
                        subresourceView.baseArrayLayer  = 0;
                        subresourceView.layerCount      = 1;

                        VkImageViewCreateInfo viewInfo {};

                        viewInfo.sType                  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                        viewInfo.image                  = m_swapchainImages[i];
                        viewInfo.viewType               = VK_IMAGE_VIEW_TYPE_2D;
                        viewInfo.format                 = m_swapchainProps.imageFormat;
                        viewInfo.components.r           = VK_COMPONENT_SWIZZLE_IDENTITY;
                        viewInfo.components.g           = VK_COMPONENT_SWIZZLE_IDENTITY;
                        viewInfo.components.b           = VK_COMPONENT_SWIZZLE_IDENTITY;
                        viewInfo.components.a           = VK_COMPONENT_SWIZZLE_IDENTITY;
                        viewInfo.subresourceRange       = subresourceView;

                        VkImageView currImageView = VK_NULL_HANDLE;
                        if( vkCreateImageView(m_context.getLogicalDevice(), &viewInfo, nullptr, &currImageView) != VK_SUCCESS )
                        {
                                LOG_ERROR("Renderer::init() failed: cannot create view for swapchain image, vkCreateImageView() failed!");
                                false;
                        }

                        m_swapchainImagesViews.push_back(currImageView);
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

                // Destroy views for the swapchain images
                for(VkImageView& view : m_swapchainImagesViews)
                        vkDestroyImageView(m_context.getLogicalDevice(), view, nullptr);

                m_swapchainImagesViews.clear();

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

                m_depthAttachmentFormat = VK_FORMAT_UNDEFINED;

                // Search a format that supports depth and stencil test too
                for(VkFormat fmt : candidates)
                {
                        VkFormatProperties2 fmtProps {};
                        fmtProps.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

                        vkGetPhysicalDeviceFormatProperties2(m_context.getPhysicalDevice(), fmt, &fmtProps);

                        if(fmtProps.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                        {
                                m_depthAttachmentFormat = fmt;
                                break;
                        }
                }

                if(m_depthAttachmentFormat == VK_FORMAT_UNDEFINED)
                {
                        LOG_ERROR("renderer::init() failed: createDepthAttachment() failed, cannot find a valid format to create the depth attachment!");
                        return false;
                }

                // Create image for the depth attachment
                if( !createImage(m_depthAttachment, m_swapchainProps.extent.width, m_swapchainProps.extent.height, 1,
                                        VK_IMAGE_TYPE_2D, 1, m_depthAttachmentFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) )
                {
                        LOG_ERROR("renderer::init() failed: createDepthAttachment() failed, creation of depth attachment image failed!");
                        return false;
                }

                // Create view into the depth attachment image
                VkImageViewCreateInfo imageViewInfo {};
                imageViewInfo.sType     = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imageViewInfo.image     = m_depthAttachment.handle;
                imageViewInfo.viewType  = VK_IMAGE_VIEW_TYPE_2D;
                imageViewInfo.format    = m_depthAttachmentFormat;

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
                m_depthAttachmentFormat = VK_FORMAT_UNDEFINED;
        }



        /**
         * @brief Renderer::createImage
         * Creates a VkImage with the given parameteres and allocates memory for it
         * @param image Output variable in which data for the created image will be stored
         * @param width Width of the image to be crated, expressed in pixels
         * @param height Height of the image to be crated, expressed in pixels
         * @param depth Depth of the image to be crated (intended as num. of layers for the image)
         * @param type Type of image to be created
         * @param mipLevels number of mip-map levels 
         * @param format Format of the texels that will compose the image to be created
         * @param tilingMode Specifies how the texels of the image should be arranged in memory
         * @param usageFlags Flags describing how the image will be used
         * @return True on success, false on failure
         *
        */
        bool Renderer::createImage(Image& image, uint32_t width, uint32_t height, uint32_t depth, VkImageType type, uint32_t mipLevels,
                        VkFormat format, VkImageTiling tilingMode, VkImageUsageFlags usageFlags)
        {
                VkImageCreateInfo imageInfo {};
                imageInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.imageType             = type;
                imageInfo.format                = format;
                imageInfo.extent.width          = width;
                imageInfo.extent.height         = height;
                imageInfo.extent.depth          = depth;
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


        /**
         * @brief Renderer::RenderPass
         * Creates the main render pass used by the renderer
         * @return True on success, false otherwise
         * @note VkRenderPass is a vulkan object that describes the attachments to be used during rendering
         *      operations and how those are used.
         *
         *      A render pass is splitted into sub-passes.
         *      Subpasses are subsequent rendering operations that depends on the content of the framebuffer computed by a
         *      previous subpass (you can see a subpass as a single step during the rendering operation).
         *
         *      The render pass object desribes dependencies between the different subpasses too.
         *
         * Render passes have been deprecated since Vulkan 1.4, but I decided to use them anyway to 
         * understand the concepts and because I don't want to force the usage of a specific Vulkan version
        */
        bool Renderer::createRenderPass()
        {
                // Describe the attachments to be used during the render pass
                VkAttachmentDescription attachmentDescriptions[2] = {};

                // Color attachment description
                attachmentDescriptions[0].format                = m_swapchainProps.imageFormat;
                attachmentDescriptions[0].samples               = VK_SAMPLE_COUNT_1_BIT;
                attachmentDescriptions[0].loadOp                = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachmentDescriptions[0].storeOp               = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescriptions[0].stencilLoadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescriptions[0].stencilStoreOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDescriptions[0].initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;
                attachmentDescriptions[0].finalLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                // Depth attachment description
                attachmentDescriptions[1].format                = m_depthAttachmentFormat;
                attachmentDescriptions[1].samples               = VK_SAMPLE_COUNT_1_BIT;
                attachmentDescriptions[1].loadOp                = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachmentDescriptions[1].storeOp               = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDescriptions[1].stencilLoadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescriptions[1].stencilStoreOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDescriptions[1].initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;
                attachmentDescriptions[1].finalLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                // Reference to the color attachment
                VkAttachmentReference colorAttachmentRef {};
                colorAttachmentRef.attachment   = 0;
                colorAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                // Reference to the depth attachment
                VkAttachmentReference depthAttachmentRef {};
                depthAttachmentRef.attachment   = 1;
                depthAttachmentRef.layout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                // Create structs to describe the subpasses, we use only 1 subpass
                VkSubpassDescription subpassDescription {};

                subpassDescription.pipelineBindPoint            = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpassDescription.colorAttachmentCount         = 1;
                subpassDescription.pColorAttachments            = &colorAttachmentRef;
                subpassDescription.pDepthStencilAttachment      = &depthAttachmentRef;

                // Create the main render pass object
                VkRenderPassCreateInfo renderPassInfo {};

                renderPassInfo.sType            = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                renderPassInfo.attachmentCount  = 2;
                renderPassInfo.pAttachments     = attachmentDescriptions;
                renderPassInfo.subpassCount     = 1;
                renderPassInfo.pSubpasses       = &subpassDescription;

                if( vkCreateRenderPass(m_context.getLogicalDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::init() failed: failed to create main render pass, vkCreateRenderPass() failed!");
                        return false;
                }

                return true;
        }


        /**
         * @brief Renderer::destroyRenderPass
         * Destroys the main render pass used by the renderer
        */
        void Renderer::destroyRenderPass()
        {
                if(m_renderPass == VK_NULL_HANDLE)
                        return;

                vkDestroyRenderPass(m_context.getLogicalDevice(), m_renderPass, nullptr);
                m_renderPass = VK_NULL_HANDLE;
        }


        /**
         * @brief Renderer::createGraphicsPipeline
         * Creates the graphics pipeline to be used by the renderer.
         * @return True on success, false on failure
         * @note The VkGraphicsPipeline is a vulkan object used to describe all the stages (both
         * programmable and fixed function stages) of the graphics pipeline.
         * Such object is used to configure (set) the GPU hardware before invoking rendering operations.
        */
        bool Renderer::createGraphicsPipeline()
        {
                // TODO: Implement a public API to manage the configuration of graphics pipeline stages and shaders

                VulkanShader vertShader(&m_context);
                VulkanShader fragShader(&m_context);

                vertShader.loadFromBinary("../resources/shaders/bin/vertexShader.spv", VulkanShader::Type::VERTEX);
                fragShader.loadFromBinary("../resources/shaders/bin/fragmentShader.spv", VulkanShader::Type::FRAGMENT);

                if( !vertShader.isValid() || !fragShader.isValid() )
                {
                        LOG_ERROR("Renderer::init() failed: failed to create graphics pipeline, vertex/fragment shader loading failed!");
                        return false;
                }

                // Create structs to specify the programmable stages of the pipeline
                VkPipelineShaderStageCreateInfo shaderStagesInfo[2] = {};

                shaderStagesInfo[0].sType       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                shaderStagesInfo[0].stage       = VK_SHADER_STAGE_VERTEX_BIT;
                shaderStagesInfo[0].module      = vertShader.getModule();
                shaderStagesInfo[0].pName       = "main";

                shaderStagesInfo[1].sType       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                shaderStagesInfo[1].stage       = VK_SHADER_STAGE_FRAGMENT_BIT;
                shaderStagesInfo[1].module      = fragShader.getModule();
                shaderStagesInfo[1].pName       = "main";

                // ====================[ Vertex input description (fixed function stage) ]====================
                // This struct describes how vertices are passed as input to the vertex shader, this is similar to glVertexAttribPointer() call in OpenGL API
                VkPipelineVertexInputStateCreateInfo vertexInputInfo {};

                vertexInputInfo.sType                                   = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputInfo.vertexBindingDescriptionCount           = 0;
                vertexInputInfo.pVertexBindingDescriptions              = nullptr;
                vertexInputInfo.vertexAttributeDescriptionCount         = 0;
                vertexInputInfo.pVertexAttributeDescriptions            = nullptr;

                // ====================[ Input assembly stage (fixed function stage) ]====================
                // This stage describes how vertices are grouped toghether to create primitives
                VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo {};

                inputAssemblyInfo.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                inputAssemblyInfo.topology                              = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                inputAssemblyInfo.primitiveRestartEnable                = VK_FALSE;
                
                // ====================[ Viewport and scissor region description (fixed function stage) ]====================
                // The viewport struct describes the dimensions of the viewport (the viewport defines how a rendered image is mapped onto the framebuffer).
                // The Scissor struct instead describes the region of the framebuffer to which pixels can be written
                VkViewport viewport {};

                viewport.x              = 0.0f;
                viewport.y              = 0.0f;
                viewport.width          = static_cast<float>( m_swapchainProps.extent.width );
                viewport.height         = static_cast<float>( m_swapchainProps.extent.height );
                viewport.minDepth       = 0.0f;
                viewport.maxDepth       = 1.0f;
                
                VkRect2D scissor {};

                scissor.offset          = { 0, 0 };
                scissor.extent          = m_swapchainProps.extent;
                
                VkPipelineViewportStateCreateInfo viewportInfo {};

                viewportInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportInfo.viewportCount      = 1;
                viewportInfo.pViewports         = &viewport;
                viewportInfo.scissorCount       = 1;
                viewportInfo.pScissors          = &scissor;

                // ====================[ Rasterizer stage (fixed function stage) ]====================
                // This stage describes:
                //      - how the primitive shapes produced by the GPU shall be rasterized (transformed into fragments)
                //      - how depth test shall be performed on the produced fragments
                //      - if, and how, face culling shall be performed
                //      - how scissor test shall be performed
                
                VkPipelineRasterizationStateCreateInfo rasterizerInfo {};

                rasterizerInfo.sType                            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                rasterizerInfo.depthClampEnable                 = VK_FALSE;
                rasterizerInfo.rasterizerDiscardEnable          = VK_FALSE;
                rasterizerInfo.polygonMode                      = VK_POLYGON_MODE_FILL;
                rasterizerInfo.cullMode                         = VK_CULL_MODE_BACK_BIT;
                rasterizerInfo.frontFace                        = VK_FRONT_FACE_CLOCKWISE;
                rasterizerInfo.depthBiasEnable                  = VK_FALSE;
                rasterizerInfo.lineWidth                        = 1.0f;

                // ====================[ Multisampling state description (fixed function stage) ]====================
                // This struct describes if multisampling is enabled and how it shall be performed
                VkPipelineMultisampleStateCreateInfo multisamplingInfo {};

                multisamplingInfo.sType                         = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                multisamplingInfo.sampleShadingEnable           = VK_FALSE;
                multisamplingInfo.rasterizationSamples          = VK_SAMPLE_COUNT_1_BIT;
                multisamplingInfo.minSampleShading              = 1.0f;
                multisamplingInfo.pSampleMask                   = nullptr;
                multisamplingInfo.alphaToCoverageEnable         = VK_FALSE;
                multisamplingInfo.alphaToOneEnable              = VK_FALSE;

                // ====================[ Depth/stencil test description (fixed function stage) ]====================
                // This struct describes if depth/stencil test is enabled and how it shall be performed
                VkPipelineDepthStencilStateCreateInfo depthStencilTestInfo {};
                
                depthStencilTestInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                depthStencilTestInfo.depthTestEnable            = VK_TRUE;
                depthStencilTestInfo.depthWriteEnable           = VK_TRUE;
                depthStencilTestInfo.depthCompareOp             = VK_COMPARE_OP_LESS_OR_EQUAL;
                
                // ====================[ Color blending stage description (fixed function stage) ]====================
                // This stage describes how color blending (blend between the color returned from the fragment shader and the color 
                // stored in the framebuffer for a specific pixel) shall be performed
                
                // This struct describes how color blending should be performed on a specific framebuffer attachment (you may have multiple framebuffers)
                VkPipelineColorBlendAttachmentState colorBlendAttachment {};

                colorBlendAttachment.colorWriteMask             = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                colorBlendAttachment.blendEnable                = VK_FALSE;
                colorBlendAttachment.srcColorBlendFactor        = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstColorBlendFactor        = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.colorBlendOp               = VK_BLEND_OP_ADD;
                colorBlendAttachment.srcAlphaBlendFactor        = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstAlphaBlendFactor        = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.alphaBlendOp               = VK_BLEND_OP_ADD;

                // This struct describes all framebuffers on which the GPU shall execute color blendingn and how (for each of them)
                VkPipelineColorBlendStateCreateInfo colorBlendingInfo {};

                colorBlendingInfo.sType                         = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                colorBlendingInfo.logicOpEnable                 = VK_FALSE;
                colorBlendingInfo.logicOp                       = VK_LOGIC_OP_COPY;
                colorBlendingInfo.attachmentCount               = 1;
                colorBlendingInfo.pAttachments                  = &colorBlendAttachment;
                
                // ====================[ Dynamic state description ]====================
                // This struct describes the portions of the pipeline state that can be changed dynamically (without recreating the entire pipeline, before a draw call is issued).
                // This is a Vulkan specific thing, it does not map to a specific graphics pipeline stage (conceptually)
                VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };

                VkPipelineDynamicStateCreateInfo dynamicStateInfo {};

                dynamicStateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicStateInfo.dynamicStateCount      = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
                dynamicStateInfo.pDynamicStates         = dynamicStates;

                // ====================[ Pipeline layout definition ]====================
                // The VkPipelineLayout object describes the resources that will be accessed by the programmable stages of a graphics pipeline.
                // Such resources are described using "descriptor set layout" structs
                VkPipelineLayoutCreateInfo pipelineLayoutInfo {};

                pipelineLayoutInfo.sType                        = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.setLayoutCount               = 0;
                pipelineLayoutInfo.pSetLayouts                  = nullptr;
                pipelineLayoutInfo.pushConstantRangeCount       = 0;
                pipelineLayoutInfo.pPushConstantRanges          = nullptr;

                if( vkCreatePipelineLayout(m_context.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS )
                {
                        LOG_ERROR("renderer::init() failed: cannot create pipeline layout, vkCreatePipelineLayout() failed!");
                        return false;
                }

                // ====================[ Pipeline creation ]====================
                // Now that we have defined all the programmable and fixed stages of the pipeline and
                // theire properties we can finally create the graphics pipeline
                VkGraphicsPipelineCreateInfo pipelineInfo {};

                pipelineInfo.sType                      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipelineInfo.stageCount                 = 2;
                pipelineInfo.pStages                    = shaderStagesInfo;
                pipelineInfo.pVertexInputState          = &vertexInputInfo;
                pipelineInfo.pInputAssemblyState        = &inputAssemblyInfo;
                pipelineInfo.pViewportState             = &viewportInfo;
                pipelineInfo.pRasterizationState        = &rasterizerInfo;
                pipelineInfo.pMultisampleState          = &multisamplingInfo;
                pipelineInfo.pDepthStencilState         = &depthStencilTestInfo; 
                pipelineInfo.pColorBlendState           = &colorBlendingInfo;
                pipelineInfo.pDynamicState              = &dynamicStateInfo;
                pipelineInfo.layout                     = m_pipelineLayout;
                pipelineInfo.renderPass                 = m_renderPass;
                pipelineInfo.subpass                    = 0;
                pipelineInfo.basePipelineHandle         = VK_NULL_HANDLE;
                pipelineInfo.basePipelineIndex          = -1;
                
                if( vkCreateGraphicsPipelines(m_context.getLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS )
                {
                        LOG_ERROR("renderer::init() failed: cannot create pipeline, vkCreateGraphicsPipeline() failed!");
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
                if(m_pipeline!= VK_NULL_HANDLE)
                {
                        vkDestroyPipeline(m_context.getLogicalDevice(), m_pipeline, nullptr);
                        m_pipeline = VK_NULL_HANDLE;
                }

                if(m_pipelineLayout != VK_NULL_HANDLE)
                {
                        vkDestroyPipelineLayout(m_context.getLogicalDevice(), m_pipelineLayout, nullptr);
                        m_pipelineLayout = VK_NULL_HANDLE;
                }
        }


        /**
         * @brief Renderer::createFramebuffers
         * Creates framebuffer objects used by the renderer to bind the swapchain
         * images (as color attachments) before rendering.
         * @return True on success, false on failure
         *
         * @note A VkFramebuffer object represents the collection of memory attachments
         * (color buffer, depth buffer, ...) that a render pass instance will target during
         * rendering operations.
         * The VkFramebuffer itself does not own any image/GPU memory, it acts only as a 
         * container to references to images/GPU memory.
         * A given VkFramebuffer can be used with any render pass as long as it matches
         * the attachments required by such rendere pass.
        */
        bool Renderer::createFramebuffers()
        {
                m_swapchainFramebuffers.resize(m_swapchainImagesViews.size());

                for(size_t i = 0; i < m_swapchainImagesViews.size(); ++i)
                {
                        // Attachments to be bound by the current framebuffer
                        VkImageView attachments[] = {
                                m_swapchainImagesViews[i],
                                m_depthAttachmentView
                        };

                        VkFramebufferCreateInfo framebufferInfo {};

                        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                        framebufferInfo.renderPass      = m_renderPass;
                        framebufferInfo.attachmentCount = 2;
                        framebufferInfo.pAttachments    = attachments;
                        framebufferInfo.width           = m_swapchainProps.extent.width;
                        framebufferInfo.height          = m_swapchainProps.extent.height;
                        framebufferInfo.layers          = 1;

                        if( vkCreateFramebuffer(m_context.getLogicalDevice(), &framebufferInfo, nullptr, &(m_swapchainFramebuffers[i]) ) != VK_SUCCESS )
                        {
                                LOG_ERROR("renderer::init() failed: cannot create framebuffers, vkCreateFramebuffers() failed!");
                                return false;
                        }
                }

                return true;
        }


        /**
         * @brief Renderer::destroyFramebuffers()
         * Destroys the framebuffers used by the renderer
        */
        void Renderer::destroyFramebuffers()
        {
                for(VkFramebuffer& framebuffer : m_swapchainFramebuffers)
                        vkDestroyFramebuffer(m_context.getLogicalDevice(), framebuffer, nullptr);

                m_swapchainFramebuffers.clear();
        }



}


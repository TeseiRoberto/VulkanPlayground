
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
                // TODO: Implement a public API to manage the graphics pipeline stages configuration and shaders

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

                colorBlendAttachment.colorWriteMask             = VK_COLOR_COMPONENT_R_BIT || VK_COLOR_COMPONENT_G_BIT || VK_COLOR_COMPONENT_B_BIT || VK_COLOR_COMPONENT_A_BIT;
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

                // TODO: Continue implementation...

                return true;
        }


        void Renderer::destroyGraphicsPipeline()
        {
                if(m_pipelineLayout != VK_NULL_HANDLE)
                {
                        vkDestroyPipelineLayout(m_context.getLogicalDevice(), m_pipelineLayout, nullptr);
                        m_pipelineLayout = VK_NULL_HANDLE;
                }

                // TODO: Continue implementation...
        }

}


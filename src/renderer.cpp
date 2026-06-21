
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
         * Initializes the renderer
         * @param wnd Window in which the renderer will draw
         * @return true on success, false on failure
         * @note GLFW must be initialized before this method is called!
        */
        bool Renderer::init(GLFWwindow* wnd)
        {
                #define CHECK(x) if( !x ) { terminate(); return false; }

                if( !createInstance() )
                        return false;

                CHECK( pickPhysicalDevice() )
                CHECK( loadQueueFamilyIndices() )
                CHECK( createLogicalDevice() )
                CHECK( createSurface(wnd) )
                CHECK( createSwapchain(wnd) )
                CHECK( createDepthAttachment() )

                // TODO: Add other stuff...
                
                return true;
        }


        /**
         * @brief Renderer::terminate
         * Terminates the renderer
        */
        void Renderer::terminate()
        {
                // TODO: Add other stuff...

                destroyDepthAttachment();
                destroySwapchain();
                destroySurface();
                destroyLogicalDevice();
                unloadQueueFamilyIndices();
                destroyInstance();
        }


        /**
         * @brief Renderer::createInstance
         * Initializes the vulkan instance to be used by the renderer.
         * The instance is the connection between the application and the Vulkan API (driver)
         * @return True on success, false on failure
        */
        bool Renderer::createInstance()
        {
                VkApplicationInfo appInfo = {};
                appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                appInfo.pApplicationName = "vulkanPlayground";
                appInfo.apiVersion = VK_API_VERSION_1_3;

                // Get instance extensions required
                uint32_t extensionCount = 0;
                const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

                if(extensions == NULL)
                {
                        LOG_ERROR("Renderer::init() failed: glfwGetRequiredInstanceExtension() returned NULL");
                        return false;
                }

                VkInstanceCreateInfo instanceInfo = {};
                instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instanceInfo.pApplicationInfo = &appInfo;
                instanceInfo.enabledExtensionCount = extensionCount;
                instanceInfo.ppEnabledExtensionNames = extensions;

                if( vkCreateInstance(&instanceInfo, nullptr, &m_instance) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::init() failed: VkCreateInstance() failed");
                        return false;
                }

                return true;
        }


        /**
         * @brief Renderer::destroyInstance
         * Terminates the vulkan instance used by the renderer
        */
        void Renderer::destroyInstance()
        {
                if(m_instance == VK_NULL_HANDLE)
                        return;

                vkDestroyInstance(m_instance, nullptr);

                m_physDevice = VK_NULL_HANDLE;
                m_instance = VK_NULL_HANDLE;
        }


        /**
         * @brief Renderer::pickPhysicalDevice
         * Enumerates physical devices and selects one to be used by the renderer
         * The physical device is a vulkan object that models a physical GPU supporting vulkan API
         * @return True if a physical device has been chosen, false otherwise
         * @note The physical device gets released when the VkInstance is destroyed, so there is no
         * "releasePhysicalDevice" method in the Renderer class.
        */
        bool Renderer::pickPhysicalDevice()
        {
                // Strings used to log device type (indexed using the VkPhysicalDeviceType enum)
                static const char* DEVICE_TYPE_STR[] = {
                        "unknown",              // VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
                        "integrated GPU",       // VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
                        "discrete GPU",         // VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
                        "virtual GPU",          // VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
                        "CPU simulated GPU"     // VK_PHYSICAL_DEVICE_TYPE_CPU = 4,
                };

                uint32_t deviceCount = 0;

                if( vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::init() failed: cannot get number of available physical devices, vkEnumeratePhysicalDevices() failed!");
                        return false;
                }

                if(deviceCount == 0)
                {
                        LOG_ERROR("Renderer::init() failed: no physical device is available!");
                        return false;
                }

                std::vector<VkPhysicalDevice> devices(deviceCount);
                if( vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data()) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::init() failed: cannot get physical devices, vkEnumeratePhysicalDevices() failed!");
                        return false;
                }

                // Pick the first available device
                m_physDevice = devices[0];

                // Log details about all the available physical devices (just for debug purpouses)
                LOG_INFO("Available physical devices:");
                for(auto& device : devices)
                {
                        VkPhysicalDeviceProperties2 props;
                        props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

                        vkGetPhysicalDeviceProperties2(device, &props);
                        
                        LOG_INFO("Physical device:");
                        LOG_INFO("    - vendor ID: %u",   props.properties.vendorID);
                        LOG_INFO("    - device ID: %u",   props.properties.deviceID);
                        LOG_INFO("    - device name: %s", props.properties.deviceName);
                        LOG_INFO("    - device type: %s", DEVICE_TYPE_STR[props.properties.deviceType]);
                }

                return true;
        }


        /**
         * @brief Renderer::loadQueueFamilyIndices
         * Queries queue families supported by the physical device and tries to load indices to
         * the family queues required by the renderer
         * @return True if the physical device supports the queue families required by the handler
         * and indices to them are loaded successfully, false otherwise
        */
        bool Renderer::loadQueueFamilyIndices()
        {
                uint32_t queueFamilyCount = 0;

                // Get number of queue families supported by the physical device
                vkGetPhysicalDeviceQueueFamilyProperties(m_physDevice, &queueFamilyCount, nullptr);

                if(queueFamilyCount == 0)
                {
                        LOG_ERROR("Renderer::init() failed: physical device, does not support any queue family!");
                        return false;
                }

                std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

                // Get properties of all queue families supported by the physical device
                vkGetPhysicalDeviceQueueFamilyProperties(m_physDevice, &queueFamilyCount, queueFamilies.data());

                // Search for a queue family that support graphics operations
                for(uint32_t i = 0; i < queueFamilyCount; ++i)
                {
                        // Check if current family is a graphic queue family which supports presentation to a window too
                        if( queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                                glfwGetPhysicalDevicePresentationSupport(m_instance, m_physDevice, i) )
                        {
                                m_queueFamilyIndices.graphicQueueIndex = i;
                                break;
                        }
                }

                return (m_queueFamilyIndices.graphicQueueIndex != UINT32_MAX);
        }


        /**
         * @brief Renderer::unloadQueueFamilyIndices
         * Resets the queue family indices loaded by the renderer to dummy values
        */
        void Renderer::unloadQueueFamilyIndices()
        {
                m_queueFamilyIndices.graphicQueueIndex = UINT32_MAX;
        }


        /**
         * @brief Renderer::createLogicalDevice
         * Creates the logical device used by the renderer
         * @return True if a logical device has been created successfully, false otherwise
        */
        bool Renderer::createLogicalDevice()
        {
                // List of device extensions required by the renderer
                std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

                // Set features to be used (Vulkan 1.0, 1.2 and 1.3 versions)
                VkPhysicalDeviceVulkan12Features enabledVk12Features {};
                enabledVk12Features.sType                                       = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
                enabledVk12Features.descriptorIndexing                          = true;
                enabledVk12Features.shaderSampledImageArrayNonUniformIndexing   = true;
                enabledVk12Features.descriptorBindingVariableDescriptorCount    = true;
                enabledVk12Features.runtimeDescriptorArray                      = true;
                enabledVk12Features.bufferDeviceAddress                         = true;

                VkPhysicalDeviceVulkan13Features enabledVk13Features {};
                enabledVk13Features.sType                                       = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
                enabledVk13Features.pNext                                       = &enabledVk12Features;
                enabledVk13Features.synchronization2                            = true;
                enabledVk13Features.dynamicRendering                            = true;

                VkPhysicalDeviceFeatures enabledVk10Features {};
                enabledVk10Features.samplerAnisotropy                           = true;

                const float queuePriorities[] = { 1.0f };
                
                // Create structs to describe queues required by the renderer
                VkDeviceQueueCreateInfo queueInfo {};
                queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueInfo.queueFamilyIndex = m_queueFamilyIndices.graphicQueueIndex;
                queueInfo.queueCount = 1;
                queueInfo.pQueuePriorities = queuePriorities;

                // Create struct to describe the logical device to be created
                VkDeviceCreateInfo deviceInfo {};
                deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                deviceInfo.pNext = &enabledVk13Features;
                deviceInfo.queueCreateInfoCount = 1;
                deviceInfo.pQueueCreateInfos = &queueInfo;
                deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
                deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
                deviceInfo.pEnabledFeatures = &enabledVk10Features;

                // Create the logical device
                if( vkCreateDevice(m_physDevice, &deviceInfo, nullptr, &m_logicDevice) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::init() failed: logical device could not be created, vkCreateDevice() failed");
                        return false;
                }

                // Retrieve handle to the graphic queue
                vkGetDeviceQueue(m_logicDevice, m_queueFamilyIndices.graphicQueueIndex, 0, &m_gfxQueue);

                return true;
        }


        /**
         * @brief Renderer::destroyLogicalDevice
         * Destroys the logical device and the queues used by the renderer
        */
        void Renderer::destroyLogicalDevice()
        {
                if(m_logicDevice == VK_NULL_HANDLE)
                        return;

                vkDestroyDevice(m_logicDevice, nullptr);
                m_logicDevice = VK_NULL_HANDLE;
                m_gfxQueue = VK_NULL_HANDLE;
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

                if( glfwCreateWindowSurface(m_instance, wnd, nullptr, &m_surface) != VK_SUCCESS )
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
        
                vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
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
                if( vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physDevice, m_surface, &surfaceCaps) != VK_SUCCESS )
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

                if( vkCreateSwapchainKHR(m_logicDevice, &swapchainInfo, nullptr, &m_swapchain) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::init() failed: cannot create swapchain, vkCreateSwapchainKHR() failed!");
                        return false;
                }

                // Aqquire handles to swapchain images
                uint32_t imageCount = 0;
                if( vkGetSwapchainImagesKHR(m_logicDevice, m_swapchain, &imageCount, nullptr) != VK_SUCCESS)
                {
                        LOG_ERROR("Renderer::init() failed: cannot acquire swapchain images, vkGetSwapchainImages() failed!");
                        return false;
                }

                m_swapchainImages.resize(imageCount);
                if( vkGetSwapchainImagesKHR(m_logicDevice, m_swapchain, &imageCount, m_swapchainImages.data()) != VK_SUCCESS)
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
                vkDestroySwapchainKHR(m_logicDevice, m_swapchain, nullptr);

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

                        vkGetPhysicalDeviceFormatProperties2(m_physDevice, fmt, &fmtProps);

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
        
                if( vkCreateImageView(m_logicDevice, &imageViewInfo, nullptr, &m_depthAttachmentView) != VK_SUCCESS )
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
                        vkDestroyImageView(m_logicDevice, m_depthAttachmentView, nullptr);
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
                if( vkCreateImage(m_logicDevice, &imageInfo, nullptr, &image.handle) != VK_SUCCESS)
                {
                        LOG_ERROR("Renderer::createImage() failed: vkCreateImage() failed!");
                        return false;
                }

                // Retrieve memory requirements for the image
                VkMemoryRequirements imageMemRequirements;
                vkGetImageMemoryRequirements(m_logicDevice, image.handle, &imageMemRequirements);
        
                VkMemoryAllocateInfo memAllocInfo {};
                memAllocInfo.sType              = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.allocationSize     = imageMemRequirements.size;
                memAllocInfo.memoryTypeIndex    = findMemoryType(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
                // Allocate memory for the image
                if( vkAllocateMemory(m_logicDevice, &memAllocInfo, nullptr, &image.memory) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer::createImage() failed: vkAllocateMemory() failed!");
                        destroyImage(image);
                        return false;
                }

                // Bind memory to the image
                vkBindImageMemory(m_logicDevice, image.handle, image.memory, 0);

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
                        vkDestroyImage(m_logicDevice, image.handle, nullptr);
                        image.handle = VK_NULL_HANDLE;
                }

                // Free memory allocated for the image
                if(image.memory != VK_NULL_HANDLE)
                {
                        vkFreeMemory(m_logicDevice, image.memory, nullptr);
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
                vkGetPhysicalDeviceMemoryProperties(m_physDevice, &memProps);

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

}


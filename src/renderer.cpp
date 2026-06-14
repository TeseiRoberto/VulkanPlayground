
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
                if( !createInstance() )
                        return false;

                if( !pickPhysicalDevice() )
                {
                        terminate();
                        return false;
                }

                if( !loadQueueFamilyIndices() )
                {
                        terminate();
                        return false;
                }

                if( !createLogicalDevice() )
                {
                        terminate();
                        return false;
                }

                if( !createSurface(wnd) )
                {
                        terminate();
                        return false;
                }

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
         * Creates the surface on which the renderer will draw
         * @param wnd Window for which the surface shall be created
         * @return True on success, false on failure
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
         * @brief Renderer::createSurface
         * Creates the surface on which the renderer will draw
         * @param wnd Window for which the surface shall be created
         * @return True on success, false on failure
        */
        void Renderer::destroySurface()
        {
                if(m_surface == VK_NULL_HANDLE)
                        return;
        
                vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
                m_surface = VK_NULL_HANDLE;
        }


}


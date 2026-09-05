
/**
 * @file vulkanDevice.cpp
 * Implements the gfxp::backend::VulkanDevice class
*/

#include "vulkanDevice.hpp"

namespace gfxp::backend {


        /**
         * @brief VulkanDevice::DEVICE_TYPE_STR
         * Strings used to log device type (indexed using the VkPhysicalDeviceType enum)
        */
        const char* VulkanDevice::DEVICE_TYPE_STR[] = {
                "unknown",              // VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
                "integrated GPU",       // VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
                "discrete GPU",         // VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
                "virtual GPU",          // VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
                "CPU simulated GPU"     // VK_PHYSICAL_DEVICE_TYPE_CPU = 4,
        };



        /**
         * @brief VulkanDevice::~VulkanDevice
         * Terminates the vulkan device if not done yet
        */
        VulkanDevice::~VulkanDevice()
        {
                if(isInit())
                        terminate();
        }


        /**
         * @brief VulkanDevice::init
         * Initializes the vulkan device by creating all the basic Vk* objects necessary
         * @param instance VkInstance to be used to enumerate and pick a physical device
         * @return true on success, false on failure
         * @note GLFW must be initialized before this method is called!
        */
        bool VulkanDevice::init(VkInstance instance)
        {
                if(isInit())
                {
                        LOG_WARN("VulkanDevice::init() called but device is already initialized!");
                        return false;
                }

                if(instance == VK_NULL_HANDLE)
                {
                        LOG_WARN("VulkanDevice::init() called with invalid VkInstance!");
                        return false;
                }

                #define CHECK(x) if( !x ) { terminate(); return false; }

                CHECK( pickPhysicalDevice(instance) )
                CHECK( loadQueueFamilyIndices(instance) )
                CHECK( createLogicalDevice() )

                return true;
        }


        /**
         * @brief VulkanDevice::terminate
         * Terminates the vulkan device by destroying all objects initialized in the init method
        */
        void VulkanDevice::terminate()
        {
                // Destroy vulkan device objects
                destroyLogicalDevice();
                unloadQueueFamilyIndices();
        }


        /**
         * @brief VulkanDevice::allocateMemory
         * Allocates device memory with the given properties
         * @param requirements Struct that describes the requirements of the memory to be allocated
         * @param memFlags Bitmask of flags that describes the properties of the memory area that shall be allocated
         * @return An handle to a valid VkMemory on success, VK_NULL_HANDLE otherwise
        */
        VkDeviceMemory VulkanDevice::allocateMemory(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags flags)
        {
                VkMemoryAllocateInfo memAllocInfo {};

                memAllocInfo.sType              = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.allocationSize     = requirements.size;
                memAllocInfo.memoryTypeIndex    = findMemoryType( requirements.memoryTypeBits, flags );
        
                // Try to allocate memory
                VkDeviceMemory memoryHandle = VK_NULL_HANDLE;

                if( vkAllocateMemory(m_logicDevice, &memAllocInfo, nullptr, &memoryHandle) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanDevice::allocateMemory() failed: call to vkAllocateMemory() failed!");
                        return VK_NULL_HANDLE;
                }

                return memoryHandle;
        }


        /**
         * @brief VulkanDevice::freeMemory
         * Deallocates the specified device memory
         * @param memoryHandle Handle to the VkMemory to be freed
        */
        void VulkanDevice::freeMemory(VkDeviceMemory& memoryHandle)
        {
                if(memoryHandle == VK_NULL_HANDLE)
                        return;

                vkFreeMemory(m_logicDevice, memoryHandle, nullptr);
                memoryHandle = VK_NULL_HANDLE;
        }


        /**
         * @brief VulkanDevice::findMemoryType
         * Queries memory properties of the physical device and searches the right 
         * type of GPU memory to be used according to the given parameters
         * @param typeFilter Bit field used to specify suitable memory types
         * @param props Flags used to specify the properties that shall be satisfied by the memory type
         * @return Index of the memory type (on the physical device) that shall be used,
         *      UINT32_MAX if such memory type is not found
        */
        uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props)
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


        /**
         * @brief VulkanDevice::pickPhysicalDevice
         * Enumerates physical devices and selects one to be associated to this device instance
         * @param instance VkInstance to be used to enumerate and pick a physical device
         * @return True if a physical device has been chosen, false otherwise
         *
         * @note The physical device is a vulkan object that models a physical GPU supporting vulkan API
         * @note The physical device gets released when the VkInstance is destroyed, so there is no
         * "releasePhysicalDevice" method in the VulkanDevice class.
        */
        bool VulkanDevice::pickPhysicalDevice(VkInstance instance)
        {
                uint32_t deviceCount = 0;

                if( vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanDevice::pickPhysicalDevice() failed: cannot get number of available physical devices, vkEnumeratePhysicalDevices() failed!");
                        return false;
                }

                if(deviceCount == 0)
                {
                        LOG_ERROR("VulkanDevice::pickPhysicalDevice() failed: no physical device is available!");
                        return false;
                }

                std::vector<VkPhysicalDevice> devices(deviceCount);
                if( vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanDevice::pickPhysicalDevice() failed: cannot get physical devices, vkEnumeratePhysicalDevices() failed!");
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
                        LOG_INFO("    - device type: %s", DEVICE_TYPE_STR[ props.properties.deviceType ]);
                }

                return true;
        }


        /**
         * @brief VulkanDevice::loadQueueFamilyIndices
         * Queries queue families supported by the physical device and tries to load indices to
         * the family queues required by the device
         * @param instance VkInstance to be used to check for queue presentation support 
         * @return True if the physical device supports the queue families require
         * and indices to them are loaded successfully, false otherwise
        */
        bool VulkanDevice::loadQueueFamilyIndices(VkInstance instance)
        {
                uint32_t queueFamilyCount = 0;

                // Get number of queue families supported by the physical device
                vkGetPhysicalDeviceQueueFamilyProperties(m_physDevice, &queueFamilyCount, nullptr);

                if(queueFamilyCount == 0)
                {
                        LOG_ERROR("VulkanContext::init() failed: physical device, does not support any queue family!");
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
                                glfwGetPhysicalDevicePresentationSupport(instance, m_physDevice, i) )
                        {
                                m_queueFamilyIndices.graphicQueueIndex = i;
                                break;
                        }
                }

                return (m_queueFamilyIndices.graphicQueueIndex != UINT32_MAX);
        }


        /**
         * @brief VulkanDevice::unloadQueueFamilyIndices
         * Resets the queue family indices loaded by the device to dummy values
        */
        void VulkanDevice::unloadQueueFamilyIndices()
        {
                m_queueFamilyIndices.graphicQueueIndex = UINT32_MAX;
        }


        /**
         * @brief VulkanDevice::createLogicalDevice
         * Creates the logical device associated to the instance and retrieves handles to the necessary VkQueues
         * @return True if a logical device has been created successfully, false otherwise
        */
        bool VulkanDevice::createLogicalDevice()
        {
                // List of device extensions required by gfxp
                std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

                // TODO: We should check for support of those features and adapt if they are not available...
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
                
                // Create structs to describe queues required by the device
                VkDeviceQueueCreateInfo queueInfo {};
                queueInfo.sType                         = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueInfo.queueFamilyIndex              = m_queueFamilyIndices.graphicQueueIndex;
                queueInfo.queueCount                    = 1;
                queueInfo.pQueuePriorities              = queuePriorities;

                // Create struct to describe the logical device to be created
                VkDeviceCreateInfo deviceInfo {};
                deviceInfo.sType                        = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                deviceInfo.pNext                        = &enabledVk13Features;
                deviceInfo.queueCreateInfoCount         = 1;
                deviceInfo.pQueueCreateInfos            = &queueInfo;
                deviceInfo.enabledExtensionCount        = static_cast<uint32_t>(deviceExtensions.size());
                deviceInfo.ppEnabledExtensionNames      = deviceExtensions.data();
                deviceInfo.pEnabledFeatures             = &enabledVk10Features;

                // Create the logical device
                if( vkCreateDevice(m_physDevice, &deviceInfo, nullptr, &m_logicDevice) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanDevice::createLogicalDevice() failed: call to vkCreateDevice() failed");
                        return false;
                }

                // Retrieve handle to the graphic queue
                vkGetDeviceQueue(m_logicDevice, m_queueFamilyIndices.graphicQueueIndex, 0, &m_gfxQueue);

                return true;
        }


        /**
         * @brief VulkanDevice::destroyLogicalDevice
         * Destroys the logical device and the queues associated to this instance
        */
        void VulkanDevice::destroyLogicalDevice()
        {
                if(m_logicDevice == VK_NULL_HANDLE)
                        return;

                vkDestroyDevice(m_logicDevice, nullptr);
                m_logicDevice = VK_NULL_HANDLE;
                m_gfxQueue = VK_NULL_HANDLE;
        }


} // namespace gfxp::backend

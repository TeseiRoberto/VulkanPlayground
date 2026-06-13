
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
         * @return true on success, false on failure
         * @note GLFW must be initialized before this method is called!
        */
        bool Renderer::init()
        {
                if( !createInstance() )
                        return false;

                if( !pickPhysicalDevice() )
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
                destroyInstance();
        }


        /**
         * @brief Renderer::createInstance
         * @return True on success, false on failure
         * Initializes the vulkan instance to be used by the renderer.
         * The instance is the connection between the application and the Vulkan API (driver)
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
                        LOG_ERROR("Renderer::init() failed: cannot get number of available physical devices, vkEnumeratePhysicalDevices failed!");
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
                        LOG_ERROR("Renderer::init() failed: cannot get physical devices, vkEnumeratePhysicalDevices failed!");
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

}


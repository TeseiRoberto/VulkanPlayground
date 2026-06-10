
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

                return true;
        }


        /**
         * @brief Renderer::terminate
         * Terminates the renderer
        */
        void Renderer::terminate()
        {
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
                        LOG_ERROR("Renderer initialization failed, glfwGetRequiredInstanceExtension() returned NULL");
                        return false;
                }

                VkInstanceCreateInfo instanceInfo = {};
                instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instanceInfo.pApplicationInfo = &appInfo;
                instanceInfo.enabledExtensionCount = extensionCount;
                instanceInfo.ppEnabledExtensionNames = extensions;

                if( vkCreateInstance(&instanceInfo, nullptr, &m_instance) != VK_SUCCESS )
                {
                        LOG_ERROR("Renderer initialization failed, VkCreateInstance() failed");
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
                // TODO: Add implementation...
        }

}



/**
 * @file vulkanContext.hpp
 * Defines the vp::VulkanContext class
*/

#ifndef VP_VULKAN_CONTEXT_H
#define VP_VULKAN_CONTEXT_H

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "log.hpp"

namespace vp {


        /**
         * @class VulkanContext
         * Models the connection with the Vulkan API driver.
         * The context keeps track of the basic Vulkan objects such as: instance, physical device,
         * logical device and queues
        */
        class VulkanContext {
        public:
                explicit                        VulkanContext() = default;
                                                ~VulkanContext();

                bool                            init();
                void                            terminate();

                inline bool                     isInit() const          { return m_instance != VK_NULL_HANDLE; }

                inline VkInstance               getInstance()           { return m_instance; }
                inline VkPhysicalDevice         getPhysicalDevice()     { return m_physDevice; }
                inline VkDevice                 getLogicalDevice()      { return m_logicDevice; }
                inline VkQueue                  getGraphicsQueue()      { return m_gfxQueue; }

        private:


                /**
                 * @struct QueueFamilyIndices
                 * Utility struct used to keep track of indices to Queue families
                */
                struct QueueFamilyIndices {
                        uint32_t        graphicQueueIndex = UINT32_MAX;         ///< Index to the graphic queue family
                };


                bool            createInstance();
                void            destroyInstance();

                bool            pickPhysicalDevice();

                bool            loadQueueFamilyIndices();
                void            unloadQueueFamilyIndices();

                bool            createLogicalDevice();
                void            destroyLogicalDevice();


                VkInstance              m_instance = VK_NULL_HANDLE;                    ///< Vulkan instance associated to the context
                VkPhysicalDevice        m_physDevice = VK_NULL_HANDLE;                  ///< Physical device associated to the context
                VkDevice                m_logicDevice = VK_NULL_HANDLE;                 ///< Logical device associated to the context

                QueueFamilyIndices      m_queueFamilyIndices;                           ///< Indices to queue families required by the context
                VkQueue                 m_gfxQueue = VK_NULL_HANDLE;                    ///< Graphic queue to which renderers using this context submits commands
        };

}

#endif // VP_VULKAN_CONTEXT_H

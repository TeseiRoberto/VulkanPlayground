
/**
 * @file vulkanDevice.hpp
 * Declares the gfxp::backend::VulkanDevice class
*/

#ifndef GFXP_BACKEND_VULKAN_DEVICE_H
#define GFXP_BACKEND_VULKAN_DEVICE_H

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "gfxpLog.hpp"

namespace gfxp::backend {


        /**
         * @class VulkanDevice
         * Manages a VkPhysicalDevice, and the related VkDevice object,
         * enables access to VkQueue objects and allocation and deallocation of device memory.
        */
        class VulkanDevice {
        public:

                /**
                 * @struct QueueFamilyIndices
                 * Utility struct used to keep track of indices to Queue families
                */
                struct QueueFamilyIndices {
                        uint32_t        graphicQueueIndex = UINT32_MAX;         ///< Index to the graphic queue family
                };


                explicit                                VulkanDevice() = default;
                                                        ~VulkanDevice();

                                                        // Disable copies
                                                        VulkanDevice(const VulkanDevice& other) = delete;
                                                        VulkanDevice(const VulkanDevice&& other) = delete;

                                                        VulkanDevice& operator =(const VulkanDevice& other) = delete;
                                                        VulkanDevice& operator =(const VulkanDevice&& other) = delete;

                // ======================================================================
                // Device initialization and termination
                bool                                    init(VkInstance instance);
                void                                    terminate();

                bool                                    isInit() const                  { return m_logicDevice != VK_NULL_HANDLE; }

                // ======================================================================
                // Getters
                inline VkPhysicalDevice                 getPhysicalDevice()             { return m_physDevice; }
                inline VkDevice                         getLogicalDevice()              { return m_logicDevice; }
                inline VkQueue                          getGraphicsQueue()              { return m_gfxQueue; }
                inline const QueueFamilyIndices         getQueueFamilyIndices()         { return m_queueFamilyIndices; }

                VkDeviceMemory                          allocateMemory(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags flags);
                void                                    freeMemory(VkDeviceMemory& memoryHandle);

                uint32_t                                findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);

        private:
                bool                    pickPhysicalDevice(VkInstance instance);

                bool                    loadQueueFamilyIndices(VkInstance instance);
                void                    unloadQueueFamilyIndices();

                bool                    createLogicalDevice();
                void                    destroyLogicalDevice();


                VkPhysicalDevice        m_physDevice = VK_NULL_HANDLE;                  ///< Physical device associated the instance
                VkDevice                m_logicDevice = VK_NULL_HANDLE;                 ///< Logical device associated to the instance

                QueueFamilyIndices      m_queueFamilyIndices;                           ///< Indices to queue families availabe for the device
                VkQueue                 m_gfxQueue = VK_NULL_HANDLE;                    ///< Handle to the graphic queue of the device


                static const char*      DEVICE_TYPE_STR[];                              ///< Human readable strings used to log device type (indexed using the VkPhysicalDeviceType enum)
        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_DEVICE_H

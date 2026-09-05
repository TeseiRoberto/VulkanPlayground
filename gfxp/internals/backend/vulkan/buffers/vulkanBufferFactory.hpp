
/**
 * @file vulkanBufferFactory.hpp
 * Declares the gfxp::backend::VulkanBufferFactory class
*/

#ifndef GFXP_BACKEND_VULKAN_BUFFER_FACTORY_H
#define GFXP_BACKEND_VULKAN_BUFFER_FACTORY_H

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <vulkan/vulkan.h>

#include "gfxpEnums.hpp"

namespace gfxp::backend {

        // Forward declarations
        class VulkanDevice;
        struct VulkanBuffer;
        struct VulkanStagingBuffer;


        /**
         * @class VulkanBufferFactory 
         * Class responsible for the creation and destruction of VulkanBuffer and VulkanStagingBuffer objects
        */
        class VulkanBufferFactory {
        public:
                explicit                VulkanBufferFactory(VulkanDevice& device) : m_device(device) {}
                                        ~VulkanBufferFactory() = default;

                VulkanBuffer*           createBuffer(gfxp::BufferUsageFlags bufferUsage, size_t capacity);
                VulkanStagingBuffer*    createStagingBuffer(size_t capacity);

                static void             destroyBuffer(VulkanBuffer*& buffer);
                static void             destroyStagingBuffer(VulkanStagingBuffer*& stagingBuffer);

        private:
                VkBuffer                createVkBuffer(VkBufferUsageFlags usage, VkDeviceSize capacity, VkMemoryRequirements& memRequirements);
                static void             destroyVkBuffer(VulkanDevice& device, VkBuffer& bufferHandle);

                VkDeviceMemory          allocateMemory(VkBuffer bufferHandle, VkMemoryPropertyFlags memFlags);
                static void             freeMemory(VulkanDevice& device, VkDeviceMemory& memoryHandle);


                VulkanDevice&          m_device;                ///< Device for which the factory will create resources for
        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_BUFFER_FACTORY_H

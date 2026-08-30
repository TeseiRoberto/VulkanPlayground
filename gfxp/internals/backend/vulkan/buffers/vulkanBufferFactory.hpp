
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
        class VulkanContext;
        struct VulkanBuffer;
        struct VulkanStagingBuffer;


        /**
         * @class VulkanBufferFactory 
         * Class responsible for the creation and destruction of VulkanBuffer and VulkanStagingBuffer objects
        */
        class VulkanBufferFactory {
        public:
                explicit                VulkanBufferFactory(VulkanContext& context) : m_context(context) {}
                                        ~VulkanBufferFactory() = default;

                VulkanBuffer*           createBuffer(gfxp::BufferType type, size_t capacity);
                VulkanStagingBuffer*    createStagingBuffer(size_t capacity);

                static void             destroyBuffer(VulkanBuffer*& buffer);
                static void             destroyStagingBuffer(VulkanBuffer*& buffer);

        private:

                VkBuffer                createBuffer(gfxp::BufferType type, size_t capacity);
                void                    destroyBuffer(VulkanContext& context, VkBuffer& bufferHandle);

                VkMemory                allocateMemory(VkBuffer bufferHandle, gfxp::BufferType type, size_t& capacity);
                void                    freeMemory(VulkanContext& context, VkMemory& memoryHandle)

                VulkanContext&          m_context;              ///< Graphic context for which the factory will create resources for
        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_BUFFER_FACTORY_H

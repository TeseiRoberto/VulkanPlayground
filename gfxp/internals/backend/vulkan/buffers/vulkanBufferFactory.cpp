
/**
 * @file vulkanBufferFactory.cpp
 * Implements the gfxp::backend::VulkanBufferFactory class
*/

#include "vulkanBufferFactory.hpp"
#include "internals/backend/vulkan/vulkanDevice.hpp"
#include "internals/backend/vulkan/vulkanEnumTranslator.hpp"
#include "vulkanBuffer.hpp"

namespace gfxp::backend {


        /**
         * @brief VulkanBufferFactory::createBuffer
         * Tries to create a VulkanBuffer object with the given properties
         * @param bufferUsage Bitmask of flags that describes the ways in which the buffer will be used
         * @param capacity Amount of memory that shall be allocated for the buffer, expressed in bytes
         * @return Pointer to a valid VulkanBuffer object on success, nullptr otherwise
        */
        VulkanBuffer* VulkanBufferFactory::createBuffer(gfxp::BufferUsageFlags bufferUsage, size_t capacity)
        {
                VkBufferUsageFlags usageFlags;
                VkMemoryPropertyFlags memPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

                if(bufferUsage & gfxp::BufferUsage::STAGING_BUFFER)
                {
                        LOG_ERROR("VulkanBufferFactory::createBuffer() failed: to create a staging buffer VulkanBufferFactory::createStagingBuffer() must be used!");
                        return nullptr;
                }

                if( !VulkanEnumTranslator::translate(bufferUsage, usageFlags) )
                {
                        LOG_ERROR("VulkanBufferFactory::createBuffer() failed: cannot convert BufferUsage to Vulkan enum value!");
                        return nullptr;
                }

                // Try to create the VkBuffer object
                VkMemoryRequirements bufferMemRequirements;
                VkBuffer bufferHandle = createVkBuffer(usageFlags, static_cast<VkDeviceSize>(capacity), bufferMemRequirements);

                if(bufferHandle == VK_NULL_HANDLE)
                        return nullptr;

                // Try to allocate memory and bind it to the buffer
                VkDeviceMemory memoryHandle = m_device.allocateMemory(bufferMemRequirements, memPropertyFlags);

                if(memoryHandle == VK_NULL_HANDLE)
                {
                        destroyVkBuffer(m_device, bufferHandle);
                        return nullptr;
                }

                // Create the VulkanBuffer instance
                VulkanBuffer* buffer = new VulkanBuffer(m_device);

                buffer->handle          = bufferHandle;
                buffer->memory          = memoryHandle;
                buffer->size            = 0;
                buffer->capacity        = capacity;

                return buffer;
        }


        /**
         * @brief VulkanBufferFactory::createStagingBuffer
         * Tries to create a VulkanStagingBuffer object with the given properties
         * @param capacity Amount of memory that shall be allocated for the buffer, expressed in bytes
         * @return Pointer to a valid VulkanStagingBuffer object on success, nullptr otherwise
        */
        VulkanStagingBuffer* VulkanBufferFactory::createStagingBuffer(size_t capacity)
        {
                VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                VkMemoryPropertyFlags memPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ;

                // Try to create the VkBuffer object
                VkMemoryRequirements bufferMemRequirements;
                VkBuffer bufferHandle = createVkBuffer(usageFlags, static_cast<VkDeviceSize>(capacity), bufferMemRequirements);

                if(bufferHandle == VK_NULL_HANDLE)
                        return nullptr;

                // Try to allocate memory and bind it to the buffer
                VkDeviceMemory memoryHandle = m_device.allocateMemory(bufferMemRequirements, memPropertyFlags);

                if(memoryHandle == VK_NULL_HANDLE)
                {
                        destroyVkBuffer(m_device, bufferHandle);
                        return nullptr;
                }

                // Try to map the buffer memory so that CPU can access it
                void* mappedPtr = nullptr;

                if( vkMapMemory(m_device.getLogicalDevice(), memoryHandle, 0, VK_WHOLE_SIZE, 0, &mappedPtr) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanBufferFactory::createStagingBuffer() failed: cannot map memory buffer, call to vkMapMemory() failed!");
                        m_device.freeMemory(memoryHandle);
                        destroyVkBuffer(m_device, bufferHandle);

                        return nullptr;
                }

                // Create the VulkanStagingBuffer instance
                VulkanStagingBuffer* stagingBuffer = new VulkanStagingBuffer(m_device);

                stagingBuffer->handle           = bufferHandle;
                stagingBuffer->memory           = memoryHandle;
                stagingBuffer->size             = 0;
                stagingBuffer->capacity         = capacity;
                stagingBuffer->dataPtr          = static_cast<uint8_t*>(mappedPtr);

                return stagingBuffer;
        }


        /**
         * @brief VulkanBufferFactory::destroyBuffer
         * Destroys and deallocates the given VulkanBuffer object
         * @param buffer VulkanBuffer object to be destroyed 
        */
        void VulkanBufferFactory::destroyBuffer(VulkanBuffer*& buffer)
        {
                if(buffer == nullptr)
                {
                        LOG_ERROR("VulkanBufferFactory::destroyBuffer() failed: buffer is not valid!");
                        return;
                }

                if( !buffer->device.isInit() )
                {
                        LOG_ERROR("VulkanBufferFactory::destroyBuffer() failed: VulkanDevice is not initialized!");
                        return;
                }

                buffer->device.freeMemory( buffer->memory );
                destroyVkBuffer(buffer->device, buffer->handle);

                delete buffer;
                buffer = nullptr;
        }


        /**
         * @brief VulkanBufferFactory::destroyStagingBuffer
         * Destroys and deallocates the given VulkanStagingBuffer object
         * @param stagingBuffer VulkanStagingBuffer object to be destroyed 
        */
        void VulkanBufferFactory::destroyStagingBuffer(VulkanStagingBuffer*& stagingBuffer)
        {
                if(stagingBuffer == nullptr)
                {
                        LOG_ERROR("VulkanBufferFactory::destroyStagingBuffer() failed: buffer is not valid!");
                        return;
                }

                if( !stagingBuffer->device.isInit() )
                {
                        LOG_ERROR("VulkanBufferFactory::destroyStagingBuffer() failed: VulkanDevice is not initialized!");
                        return;
                }

                // Unmap buffer's memory
                if(stagingBuffer->dataPtr != nullptr)
                {
                        vkUnmapMemory(stagingBuffer->device.getLogicalDevice(), stagingBuffer->memory);
                        stagingBuffer->dataPtr = nullptr;
                }

                stagingBuffer->device.freeMemory( stagingBuffer->memory );
                destroyVkBuffer(stagingBuffer->device, stagingBuffer->handle);

                delete stagingBuffer;
                stagingBuffer = nullptr;
        }


        /**
         * @brief VulkanBufferFactory::createVkBuffer
         * Tries to create a VkBuffer object
         * @param Usage Bitmask of flags that describes how the buffer will be used
         * @param capacity Amount of memory that shall be allocated for the buffer, expressed in bytes
         * @param memRequirements Output variable in which memory requirements for the created buffer will be stored
         * @return An handle to a valid VkBuffer on success, VK_NULL_HANDLE otherwise
        */
        VkBuffer VulkanBufferFactory::createVkBuffer(VkBufferUsageFlags usage, VkDeviceSize capacity, VkMemoryRequirements& memRequirements)
        {
                if(capacity == 0)
                {
                        LOG_ERROR("VulkanBufferFactory::createVkBuffer() failed: cannot create a buffer with 0 bytes of capacity!");
                        return VK_NULL_HANDLE;
                }

                VkBufferCreateInfo bufferInfo {};

                bufferInfo.sType        = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferInfo.size         = capacity;
                bufferInfo.usage        = usage;
                bufferInfo.sharingMode  = VK_SHARING_MODE_EXCLUSIVE;            // TODO: Handle shared buffers

                // Try to create the VkBuffer object
                VkBuffer bufferHandle = VK_NULL_HANDLE;

                if( vkCreateBuffer(m_device.getLogicalDevice(), &bufferInfo, nullptr, &bufferHandle) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanBufferFactory::createVkBuffer() failed: vkCreateBuffer() failed!");
                        return VK_NULL_HANDLE;
                }

                // Retrieve memory requirements for the buffer
                vkGetBufferMemoryRequirements(m_device.getLogicalDevice(), bufferHandle, &memRequirements);

                return bufferHandle;
        }


        /**
         * @brief VulkanBufferFactory::destroyVkBuffer
         * Destroys the given VkBuffer
         * @param device Graphic device from which the buffer has been created
         * @param bufferHandle Handle to the VkBuffer to be destroyed
        */
        void VulkanBufferFactory::destroyVkBuffer(VulkanDevice& device, VkBuffer& bufferHandle)
        {
                if(bufferHandle == VK_NULL_HANDLE)
                        return;

                vkDestroyBuffer(device.getLogicalDevice(), bufferHandle, nullptr);
                bufferHandle = VK_NULL_HANDLE;
        }


} // namespace gfxp::backend

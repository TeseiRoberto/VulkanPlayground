
/**
 * @file vulkanBufferFactory.cpp
 * Implements the gfxp::backend::VulkanBufferFactory class
*/

#include "vulkanBufferFactory.hpp"
#include "internals/backend/vulkan/vulkanContext.hpp"
#include "internals/backend/vulkan/vulkanEnumTranslator.hpp"
#include "vulkanBuffer.hpp"

namespace gfxp::backend {


        /**
         * @brief VulkanBufferFactory::createBuffer
         * Tries to create a VulkanBuffer object with the given properties
         * @param type Type of buffer to be created
         * @param capacity Amount of memory that shall be allocated for the buffer, expressed in bytes
         * @return Pointer to a valid VulkanBuffer object on success, nullptr otherwise
        */
        VulkanBuffer* VulkanBufferFactory::createBuffer(gfxp::BufferType type, size_t capacity)
        {
                if(type == gfxp::BufferType::UNKNOWN_BUFFER || type == gfxp::BufferType::STAGING_BUFFER)
                {
                        LOG_ERROR("VulkanBufferFactory::createBuffer() failed: cannot create an unknown/staging buffer!");
                        return nullptr;
                }

                // Try to create the VkBuffer object
                VkBuffer bufferHandle = createBuffer(type, capacity);

                if(bufferHandle == VK_NULL_HANDLE)
                        return nullptr;

                // Try to allocate memory and bind it to the buffer
                VkMemory memoryHandle = allocateMemory(bufferHandle, type, capacity);

                if(memoryHandle == VK_NULL_HANDLE)
                {
                        destroyBuffer(bufferHandle);
                        return nullptr;
                }

                // Create the VulkanBuffer instance
                VulkanBuffer* buffer = new VulkanBuffer(m_context);

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
                // Try to create the VkBuffer object
                VkBuffer bufferHandle = createBuffer(gfxp::BufferType::STAGING_BUFFER, capacity);

                if(bufferHandle == VK_NULL_HANDLE)
                        return nullptr;

                // Try to allocate memory and bind it to the buffer
                VkMemory memoryHandle = allocateMemory(bufferHandle, gfxp::BufferType::STAGING_BUFFER, capacity);

                if(memoryHandle == VK_NULL_HANDLE)
                {
                        destroyBuffer(bufferHandle);
                        return nullptr;
                }

                // Try to map the buffer memory so that CPU can access it
                uint8_t* mappedPtr = nullptr;

                if( vkMapMemory(m_context.getLogicalDevice(), memoryHandle, 0, VK_WHOLE_SIZE, 0, &mappedPtr) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanBufferFactory::createStagingBuffer() failed: cannot map memory buffer, vkMapMemory() failed!");
                        freeMemory(memoryHandle)
                        destroyBuffer(bufferHandle);

                        return nullptr;
                }

                // Create the VulkanStagingBuffer instance
                VulkanStagingBuffer* stagingBuffer = new VulkanStagingBuffer(m_context);

                stagingBuffer->handle           = bufferHandle;
                stagingBuffer->memory           = memoryHandle;
                stagingBuffer->size             = 0;
                stagingBuffer->capacity         = capacity;
                stagingBuffer->dataPtr          = mappedPtr;

                return stagingBuffer;
        }


        /**
         * @brief VulkanBufferFactory::destroyBuffer
         * Destroys and deallocates the given VulkanBuffer object
        */
        void VulkanBufferFactory::destroyBuffer(VulkanBuffer*& buffer)
        {
                if(buffer == nullptr)
                {
                        LOG_ERROR("VulkanBufferFactory::destroyBuffer() failed: buffer is not valid!");
                        return;
                }

                if( !buffer->context.isInit() )
                {
                        LOG_ERROR("VulkanBufferFactory::destroyBuffer() failed: VulkanContext is not initialized!");
                        return;
                }

                freeMemory(buffer->context, buffer->memory);
                destroyBuffer(buffer->context, buffer->handle);

                delete buffer;
                buffer = nullptr;
        }


        /**
         * @brief VulkanBufferFactory::destroyStagingBuffer
         * Destroys and deallocates the given VulkanStagingBuffer object
        */
        void VulkanBufferFactory::destroyStagingBuffer(VulkanStagingBuffer*& stagingBuffer)
        {
                if(stagingBuffer == nullptr)
                {
                        LOG_ERROR("VulkanBufferFactory::destroyStagingBuffer() failed: buffer is not valid!");
                        return;
                }

                if( !stagingBuffer->context.isInit() )
                {
                        LOG_ERROR("VulkanBufferFactory::destroyStagingBuffer() failed: VulkanContext is not initialized!");
                        return;
                }

                // Unmap buffer's memory
                if(stagingBuffer->dataPtr != nullptr)
                {
                        vkUnmapMemory(stagingBuffer->context.getLogicalDevice(), buffer->memory);
                        stagingBuffer->dataPtr = nullptr;
                }

                freeMemory(stagingBuffer->context, stagingBuffer->memory);
                destroyBuffer(stagingBuffer->context, stagingBuffer->handle);

                delete stagingBuffer;
                stagingBuffer = nullptr;
        }


        /**
         * @brief VulkanBufferFactory::createBuffer
         * Tries to create a VkBuffer object
         * @param type Type of buffer to be created
         * @param capacity Amount of memory that shall be allocated for the buffer, expressed in bytes
         * @return An handle to a valid VkBuffer on success, VK_NULL_HANDLE otherwise
        */
        VkBuffer VulkanBufferFactory::createBuffer(gfxp::BufferType type, size_t capacity)
        {
                VkBufferUsageFlags bufferUsage;

                if(capacity == 0)
                {
                        LOG_ERROR("VulkanBufferFactory::createBuffer() failed: cannot create a buffer with 0 bytes of capacity!");
                        return VK_NULL_HANDLE;
                }

                if( !VulkanEnumTranslator::translate(type, bufferUsage) )
                {
                        LOG_ERROR("VulkanBufferFactory::createBuffer() failed: cannot convert BufferType to Vulkan enum value!");
                        return VK_NULL_HANDLE;
                }

                VkBufferCreateInfo bufferInfo {};

                bufferInfo.sType        = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferInfo.size         = static_cast<VkDeviceSize>(capacity);
                bufferInfo.usage        = bufferUsage;
                bufferInfo.sharingMode  = VK_SHARING_MODE_EXCLUSIVE;            // TODO: Handle shared buffers

                // Try to create the VkBuffer object
                VkBuffer bufferHandle = VK_NULL_HANDLE;

                if( vkCreateBuffer(m_context.getLogicalDevice(), &bufferInfo, nullptr, &bufferHandle) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanBufferFactory::createBuffer() failed: vkCreateBuffer() failed!");
                        return VK_NULL_HANDLE;
                }

                return bufferHandle;
        }


        /**
         * @brief VulkanBufferFactory::destroyBuffer
         * Destroys the given VkBuffer
         * @param context Graphic context from which the buffer has been created
         * @param bufferHandle Handle to the VkBuffer to be destroyed
        */
        void VulkanBufferFactory::destroyBuffer(VulkanContext& context, VkBuffer& bufferHandle)
        {
                if(bufferHandle == VK_NULL_HANDLE)
                        return;

                vkDestroyBuffer(context.getLogicalDevice(), bufferHandle, nullptr);
                bufferHandle = VK_NULL_HANDLE;
        }


        /**
         * @brief VulkanBufferFactory::allocateMemory
         * Allocates memory for the specified VkBuffer
         * @param bufferHandle Handle to the VkBuffer for which memory must be allocated
         * @param type Type of buffer for which memory myst be allocated (used to determine memory properties)
         * @param capacity Amount of memory that shall be allocated for the buffer, expressed in bytes
         * @return An handle to a valid VkMemory on success, VK_NULL_HANDLE otherwise
        */
        VkMemory VulkanBufferFactory::allocateMemory(VkBuffer bufferHandle, gfxp::BufferType type, size_t& capacity)
        {
                if(bufferHandle == VK_NULL_HANDLE)
                        return VK_NULL_HANDLE;

                VkMemoryPropertyFlags memFlags;
                if( !VulkanEnumTranslator::translate(type, memFlags) )
                {
                        LOG_ERROR("VulkanBufferFactory::allocateMemory() failed: cannot convert BufferType to Vulkan enum value!");
                        return VK_NULL_HANDLE;
                }

                // Retrieve memory requirements for the buffer
                VulkanMemoryRequirements bufferMemRequirements {};
                vkGetBufferMemoryRequirements(m_context.getLogicalDevice(), bufferHandle, &bufferMemRequirements);
        
                VkMemoryAllocateInfo memAllocInfo {};

                memAllocInfo.sType              = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.allocationSize     = bufferMemRequirements.size;
                memAllocInfo.memoryTypeIndex    = m_context.findMemoryType( bufferMemRequirements.memoryTypeBits, flags );
        
                // Try to allocate memory for the buffer
                VkMemory memoryHandle = VK_NULL_HANDLE;

                if( vkAllocateMemory(m_context.getLogicalDevice(), &memAllocInfo, nullptr, &memoryHandle) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanBufferFactory::allocateMemory() failed: vkAllocateMemory() failed!");
                        return VK_NULL_HANDLE;
                }

                // Bind memory to the buffer
                vkBindBufferMemory(m_context.getLogicalDevice(), bufferHandle, memoryHandle, 0);

                return memoryHandle;
        }


        /**
         * @brief VulkanBufferFactory::freeMemory
         * Deallocates GPU memory identified by the given VkMemory
         * @param context Graphic context from which the memory has been allocated
         * @param memoryHandle Handle to the VkMemory to be freed
        */
        void VulkanBufferFactory::freeMemory(VulkanContext& context, VkMemory& memoryHandle)
        {
                if(memoryHandle == VK_NULL_HANDLE)
                        return;

                vkFreeMemory(context.getLogicalDevice(), memoryHandle, nullptr);
                memoryHandle = VK_NULL_HANDLE;
        }


} // namespace gfxp::backend

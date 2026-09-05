
/**
 * @file vulkanBuffer.hpp
 * Declares gfxp::backend::VulkanBuffer and
 * gfxp::backend::VulkanStagingBuffer structs
*/

#ifndef GFXP_BACKEND_VULKAN_BUFFER_H
#define GFXP_BACKEND_VULKAN_BUFFER_H

#include <vulkan/vulkan.h>

namespace gfxp::backend {

        class VulkanDevice;


        /**
         * @struct VulkanBuffer
         * POD struct that models a buffer object (allocated in GPU's memory) for the Vulkan API backend
         * @note The VkBuffer object defines the logical properties of a buffer (GPU memory area).
         *      In Vulkan, the actual memory area in which data gets stored must be allocated explicitly
         *      on the GPU, and then bound to the buffer object; this is done automatically by gfxp.
        */
        struct VulkanBuffer {
                VkBuffer                handle = VK_NULL_HANDLE;        ///< Handle to the buffer object
                VkDeviceMemory          memory = VK_NULL_HANDLE;        ///< Handle to the memory area allocated for the buffer
                VkDeviceSize            size = 0;                       ///< Size of the data currently stored in the buffer, expressed in bytes
                VkDeviceSize            capacity = 0;                   ///< Capacity of the buffer, expressed in bytes
                VulkanDevice&           device;                         ///< Device to which the buffer belongs to


                /**
                 * @brief VulkanBuffer::VulkanBuffer
                 * Struct constructor, creates an invalid buffer object instance
                 * @param device Graphic device that owns the buffer object
                */
                VulkanBuffer(VulkanDevice& device)
                        : handle(VK_NULL_HANDLE)
                        , memory(VK_NULL_HANDLE)
                        , size(0)
                        , capacity(0)
                        , device(device)
                {}


                /**
                 * @brief VulkanBuffer::~VulkanBuffer
                 * Virtual destructor
                */
                virtual ~VulkanBuffer() {}


                /**
                 * @brief VulkanBuffer::isValid
                 * Utility method to check if the object is correctly initialized or not
                */
                inline bool isValid()
                {
                        return (handle != VK_NULL_HANDLE && memory != VK_NULL_HANDLE);
                }
        };


        /**
         * @struct VulkanStagingBuffer
         * POD struct that models a staging buffer object for the Vulkan API backend.
         * A staging buffer is a buffer that gets used as temporary storage to upload data from CPU to GPU, or download data from GPU to CPU.
         * Such buffer object is associated to a memory area which is host visible (accessible by both CPU and GPU) and host coherent.
        */
        struct VulkanStagingBuffer : VulkanBuffer {
                uint8_t*        dataPtr = nullptr;       ///< Pointer to the mapped memory area allocated for the staging buffer


                /**
                 * @brief VulkanStagingBuffer::VulkanStagingBuffer
                 * Struct constructor, creates an invalid staging buffer object instance
                 * @param device Graphic device that owns the staging buffer object
                */
                VulkanStagingBuffer(VulkanDevice& device)
                        : VulkanBuffer(device)
                        , dataPtr(nullptr)
                {}


                /**
                 * @brief VulkanStagingBuffer::~VulkanStagingBuffer
                 * Virtual destructor
                */
                virtual ~VulkanStagingBuffer() {}
        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_BUFFER_H

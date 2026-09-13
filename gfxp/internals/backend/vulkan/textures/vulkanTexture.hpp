
/**
 * @file vulkanTexture.hpp
 * Declares gfxp::backend::VulkanTexture and
 * gfxp::backend::VulkanStagingBuffer structs
*/

#ifndef GFXP_BACKEND_VULKAN_TEXTURE_H
#define GFXP_BACKEND_VULKAN_TEXTURE_H

#include <vulkan/vulkan.h>

namespace gfxp::backend {

        class VulkanDevice;


        /**
         * @struct VulkanTexture
         * POD struct that models a texture object (allocated in GPU's memory) for the Vulkan API backend
         *
         * Keeps track of the handle to a vulkan image object and the allocated memory for the image data.
         *
         * @note The VkImage object defines the logical properties of a texture (texels format,
         *      texture size, mipmap levels), such object is used to specify how the GPU should
         *      interpret image data.
         *      The actual memory area in which image data gets stored must be allocated explicitly
         *      on the GPU, and then bound to the image object; this is done automatically by gfxp.
        */
        struct VulkanTexture {
                VkImage                 handle = VK_NULL_HANDLE;        ///< Handle to the image object
                VkDeviceMemory          memory = VK_NULL_HANDLE;        ///< Handle to the memory area allocated for the image data

                VulkanDevice&           device;                         ///< Device to which the texture belongs to


                /**
                 * @brief VulkanTexture::VulkanTexture
                 * Struct constructor, creates an invalid texture object instance
                 * @param device Device that owns the texture object
                */
                VulkanTexture(VulkanDevice& device)
                        : handle(VK_NULL_HANDLE)
                        , memory(VK_NULL_HANDLE)
                        , device(device)
                {}


                /**
                 * @brief VulkanTexture::isValid
                 * Utility method to check if the object is correctly initialized or not
                */
                inline bool isValid()
                {
                        return (handle != VK_NULL_HANDLE && memory != VK_NULL_HANDLE);
                }

        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_TEXTURE_H

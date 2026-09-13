
/**
 * @file vulkanTextureFactory.cpp
 * Implements the gfxp::backend::VulkanTextureFactory class
*/

#include "vulkanTextureFactory.hpp"
#include "internals/backend/vulkan/vulkanDevice.hpp"
#include "vulkanTexture.hpp"

namespace gfxp::backend {


        /**
         * @brief VulkanTextureFactory::createTexture
         * Tries to create a valid VulkanTexture object according to the given texture description
         * @param textureDesc Struct that describes the properties of the texture
         * @return Pointer to a valid VulkanTexture object on success, nullptr otherwise
        */
        VulkanTexture* VulkanTextureFactory::createTexture(const gfxp::TextureDescription& textureDesc)
        {
                // Try to create a VkImage
                VkMemoryRequirements imgMemRequirements {};
                VkImage imgHandle = createVkImage(textureDesc, imgMemRequirements);

                if(imgHandle == VK_NULL_HANDLE)
                        return nullptr;

                // Try to allocate memory and bind it to the image
                VkDeviceMemory memoryHandle = m_device.allocateMemory(imgMemRequirements, memPropertyFlags);

                if(memoryHandle == VK_NULL_HANDLE)
                {
                        destroyVkImage(m_device.getLogicalDevice(), imgHandle);
                        return nullptr;
                }

                vkBindImageMemory(m_device.getLogicalDevice(), imgHandle, memoryHandle, 0);

                // Create the VulkanTexture instance
                VulkanTexture* texture = new VulkanTexture(m_device);

                texture->handle         = imgHandle;
                texture->memoryHandle   = memoryHandle;

                return texture;
        }


        /**
         * @brief VulkanTextureFactory::destroyTexture
         * Destroys and deallocates the given VulkanTexture object
         * @param texture VulkanTexture object that must be destroyed
        */
        void VulkanTextureFactory::destroyTexture(VulkanTexture*& texture)
        {
                if(texture == nullptr)
                {
                        LOG_ERROR("VulkanTextureFactory::destroyTexture() failed: given texture is invalid!");
                        return;
                }

                if( !texture->device.isInit() )
                {
                        LOG_ERROR("VulkanTextureFactory::destroyTexture() failed: VulkanDevice is not initialized!");
                        return;
                }

                destroyVkImage(texture->device, texture->imgHandle);
                texture->device.freeMemory( texture->memoryHandle );

                delete texture;
                texture = nullptr;
        }


        /**
         * @brief VulkanTextureFactory::createVkImage
         * Tries to create a VkImage with the given parameters
         * @param textureDesc Struct that describes the properties of the image
         * @param memRequirements Output variable in which memory requirements for the created image will be stored
         * @return Handle to a valid VkImage on success, VK_NULL_HANDLE otherwise
        */
        VkImage VulkanTextureFactory::createVkImage(const gfxp::TextureDescription& textureDesc, VkMemoryRequirements& memRequirements)
        {
                VkImageCreateInfo imgInfo       = {};
                imgInfo.sType                   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imgInfo.extent.width            = textureDesc.getWidth();
                imgInfo.extent.height           = textureDesc.getHeight();
                imgInfo.extent.depth            = textureDesc.getDepth();
                imgInfo.mipLevels               = textureDesc.getMipLevels();
                imgInfo.arrayLayers             = textureDesc.getLayersNum();
                imgInfo.tiling                  = tilingMode;
                imgInfo.sharingMode             = VK_SHARING_MODE_EXCLUSIVE;    // TODO: Handle shared images!
                imgInfo.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;    // TODO: Handle image layouts explicitly (is it really necessary???)

                if( !VulkanEnumTranslator::translate(textureDesc.getType(), imgInfo.imageType) )
                {
                        LOG_ERROR("VulkanTextureFactory::createVkImage() failed: cannot convert TextureType to Vulkan enum value!");
                        return VK_NULL_HANDLE;
                }

                if( !VulkanEnumTranslator::translate(textureDesc.getFormat(), imgInfo.format) )
                {
                        LOG_ERROR("VulkanTextureFactory::createVkImage() failed: cannot convert TextureFormat to Vulkan enum value!");
                        return VK_NULL_HANDLE;
                }

                if( !VulkanEnumTranslator::translate(textureDesc.getSampleCount(), imgInfo.samples) )
                {
                        LOG_ERROR("VulkanTextureFactory::createVkImage() failed: cannot convert TextureSampleCount to Vulkan enum value!");
                        return VK_NULL_HANDLE;
                }

                if( !VulkanEnumTranslator::translate(textureDesc.getUsage(), imgInfo.usage) )
                {
                        LOG_ERROR("VulkanTextureFactory::createVkImage() failed: cannot convert TextureSampleCount to Vulkan enum value!");
                        return VK_NULL_HANDLE;
                }

                // Create the vulkan image object
                VkImage imgHandle = VK_NULL_HANDLE;
                if( vkCreateImage(m_device.getLogicalDevice(), &imgInfo, nullptr, &imgHandle) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanTextureFactory::createVkImage() failed: call to vkCreateImage() failed!");
                        return VK_NULL_HANDLE;
                }

                // Retrieve memory requirements for the image
                vkGetImageMemoryRequirements(m_device.getLogicalDevice(), imgHandle, &memRequirements);

                return imgHandle;
        }


        /**
         * @brief VulkanTextureFactory::destroyVkImage
         * Destroys the given VkImage
         * @param device Device from which the image has been created
         * @param imgHandle Handle to the VkImage to be destroyed
        */
        void VulkanTextureFactory::destroyVkImage(VulkanDevice& device, VkImage& imgHandle)
        {
                if(imgHandle == VK_NULL_HANDLE)
                        return;

                vkDestroyImage(device.getLogicalDevice(), imgHandle, nullptr);
                imgHandle = VK_NULL_HANDLE;
        }


} // namespace gfxp::backend

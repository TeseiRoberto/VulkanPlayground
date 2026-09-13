
/**
 * @file vulkanTextureFactory.hpp
 * Declares the gfxp::backend::VulkanTextureFactory class
*/

#ifndef GFXP_BACKEND_VULKAN_TEXTURE_FACTORY_H
#define GFXP_BACKEND_VULKAN_TEXTURE_FACTORY_H

#include <cstdint>
#include <vulkan/vulkan.h>

#include "gfxpEnums.hpp"
#include "textures/textureDescription.hpp"

namespace gfxp::backend {

        // Forward declarations
        class VulkanDevice;
        struct VulkanTexture;


        /**
         * @class VulkanTextureFactory
         * Class responsible for the creation and destruction of VulkanTexture objects
        */
        class VulkanTextureFactory {
        public:
                explicit                VulkanTextureFactory(VulkanDevice& device) : m_device(device) {}
                                        ~VulkanTextureFactory() = default;

                VulkanTexture*          createTexture(const gfxp::TextureDescription& textureDesc);
                static void             destroyTexture(VulkanTexture*& texture);

        private:
                VkImage                 createVkImage(const gfxp::TextureDescription& textureDesc, VkMemoryRequirements& memRequirements);
                static void             destroyVkImage(VulkanDevice& device, VkImage& imgHandle);


                VulkanDevice&          m_device;                ///< Device for which the factory will create resources for
        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_TEXTURE_FACTORY_H

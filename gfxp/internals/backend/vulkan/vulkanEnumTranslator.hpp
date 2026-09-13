
/**
 * @file vulkanEnumTranslator.hpp
 * Declares the gfxp::backend::VulkanEnumTranslator class
*/

#ifndef GFXP_BACKEND_VULKAN_ENUM_TRANSLATOR_H
#define GFXP_BACKEND_VULKAN_ENUM_TRANSLATOR_H

#include <vulkan/vulkan.h>
#include "gfxpEnums.hpp"

namespace gfxp::backend {


        /*!
         * @class VulkanEnumTranslator
         * Utility class used to translate API agnostic enum types to Vulkan ones
        */
        class VulkanEnumTranslator {
        public:
                                // Disable instance creation
                                VulkanEnumTranslator() = delete;
                                ~VulkanEnumTranslator() = default;

                static bool     translate(const gfxp::BufferUsageFlags from, VkBufferUsageFlags& to);

                static bool     translate(const gfxp::TextureType from, VkImageType& to);
                static bool     translate(const gfxp::TextureUsage from, VkImageUsageFlags& to);
                static bool     translate(const gfxp::TextureFormat from, VkFormat& to);
                static bool     translate(const gfxp::TextureSampleCount from, VkSampleCountFlags& to);

                static bool     translate(const ShaderType from, VkShaderStageFlagBits& to);
                static bool     translateToSize(const VertexAttributeType from, size_t& to);

                static bool     translate(const VertexAttributeType from, VkFormat& to);
                static bool     translate(const VertexInputRate from, VkVertexInputRate& to);
                static bool     translate(const PrimitiveType from, VkPrimitiveTopology& to);
                static bool     translate(const CullMode from, VkCullModeFlagBits& to);
                static bool     translate(const TriangleFrontFace from, VkFrontFace& to);
                static bool     translate(const CompareOperatorType from, VkCompareOp& to);

        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_ENUM_TRANSLATOR_H

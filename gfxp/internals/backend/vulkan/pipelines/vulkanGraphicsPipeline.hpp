
/**
 * @file vulkanGraphicsPipeline.hpp
 * Declares the gfxp::backend::VulkanGraphicsPipeline struct
*/

#ifndef GFXP_BACKEND_VULKAN_GRAPHICS_PIPELINE_H
#define GFXP_BACKEND_VULKAN_GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>

namespace gfxp::backend {

        class VulkanContext;


        /**
         * @struct VulkanGraphicsPipeline
         * POD struct that models a graphics pipeline object for the Vulkan API backend
        */
        struct VulkanGraphicsPipeline {
                VkPipeline              handle = VK_NULL_HANDLE;                ///< Handle to the vulkan pipeline object
                VkPipelineLayout        layoutHandle = VK_NULL_HANDLE;          ///< Handle to the vulkan object that describes the resources used by the pipeline

                VulkanContext&          context;                                ///< Context to which the graphics pipeline belongs to


                /**
                 * @brief VulkanGraphicsPipeline
                 * Struct constructor, creates an invalid graphics pipeline object
                 * @param context Graphic context that owns the graphics pipeline object
                */
                VulkanGraphicsPipeline(VulkanContext& context)
                        : handle(VK_NULL_HANDLE)
                        , layoutHandle(VK_NULL_HANDLE)
                        , context(context)
                {}


                /**
                 * @brief VulkanGraphicsPipeline::isValid
                 * Utility method to check if the object is correctly initialized or not
                */
                inline bool isValid()
                {
                        return (handle != VK_NULL_HANDLE && layoutHandle != VK_NULL_HANDLE);
                }
        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_GRAPHICS_PIPELINE_H

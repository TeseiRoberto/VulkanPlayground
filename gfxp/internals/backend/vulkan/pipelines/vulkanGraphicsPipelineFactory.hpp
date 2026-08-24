
/**
 * @file vulkanGraphicsPipelineFactory.hpp
 * Declares the gfxp::backend::VulkanGraphicsPipelineFactory class
*/

#ifndef GFXP_BACKEND_VULKAN_GRAPHICS_PIPELINE_FACTORY_H
#define GFXP_BACKEND_VULKAN_GRAPHICS_PIPELINE_FACTORY_H

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

#include "gfxpLog.hpp"
#include "gfxpEnums.hpp"
#include "pipelines/graphicsPipelineDescription.hpp"
#include "internals/backend/vulkan/vulkanContext.hpp"
#include "internals/backend/vulkan/vulkanEnumTranslator.hpp"
#include "internals/backend/vulkan/shaders/vulkanShader.hpp"
#include "vulkanGraphicsPipeline.hpp"

namespace gfxp::backend {


        /**
         * @brief VulkanGraphicsPipelineFactory
         * Class responsible for creation and destruction of VulkanGraphicsPipeline objects
        */
        class VulkanGraphicsPipelineFactory {
        public:
                explicit                VulkanGraphicsPipelineFactory(VulkanContext& context) : m_context(context) {}
                                        ~VulkanGraphicsPipelineFactory() = default;

                VulkanGraphicsPipeline* createPipeline(const gfxp::GraphicsPipelineDescription& pipelineDesc, VkRenderPass renderPass);
                static void             destroyPipeline(VulkanGraphicsPipeline*& pipeline);

        private:
                bool                    fillViewportInfo(const gfxp::GraphicsPipelineDescription& pipelineDesc);
                bool                    fillInputAssemblyInfo(const gfxp::PrimitiveTopologyType primitiveType);

                bool                    addShaderStageInfo(const gfxp::ShaderHandle handle);

                bool                    fillRasterizationStageInfo(const gfxp::GraphicsPipelineDescription::RasterizerDescription& rasterizerDesc);
                bool                    fillMultisamplingInfo(const gfxp::GraphicsPipelineDescription& pipelineDesc);
                bool                    fillColorBlendingInfo(const gfxp::GraphicsPipelineDescription& pipelineDesc);

                bool                    fillDepthTestInfo(const gfxp::GraphicsPipelineDescription::DepthTestDescription& depthTestDesc);

                void                    reset();


                VulkanContext&                                          m_context;                               ///< Graphic context for which the factory will create resources for

                VkViewport                                              m_viewport = {};                         ///< Struct that describes the viewport
                VkRect2D                                                m_scissorRect = {};                      ///< Struct that describes the scissor region
                VkPipelineViewportStateCreateInfo                       m_viewportInfo = {};                     ///< Struct that describes all viewports and scissor regions

                VkPipelineInputAssemblyStateCreateInfo                  m_inputAssemblyInfo = {};                ///< Struct that describes the input assembly state

                std::vector<VkPipelineShaderStageCreateInfo>            m_shaderStagesInfo = {};                 ///< Structs that describes the programmable shader stages
                VkPipelineVertexInputStateCreateInfo                    m_vertexInputInfo = {};                 ///< Struct that describes the input taken by the vertex shader (vertex binding slots and vertex attributes)

                VkPipelineRasterizationStateCreateInfo                  m_rasterizerInfo = {};                   ///< Struct that describes how rasterization of graphic primitives shall be performed
                VkPipelineMultisampleStateCreateInfo                    m_multisamplingInfo = {};                ///< Struct that describes if multisampling is enabled and how it shall be performed
        
                VkPipelineDepthStencilStateCreateInfo                   m_depthStencilTestInfo = {};             ///< Struct that describes if depth and/or stencil tests are enabled and how they shall be performed
        
                VkPipelineColorBlendStateCreateInfo                     m_colorBlendingInfo = {};                ///< Struct that describes if color blending is enabled and how it shall be performed
                std::vector<VkPipelineColorBlendAttachmentState>        m_colorBlendAttachments = {};            ///< Structs that describes how color blending shall be performed on color attachments
        };

} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_GRAPHICS_PIPELINE_FACTORY_H


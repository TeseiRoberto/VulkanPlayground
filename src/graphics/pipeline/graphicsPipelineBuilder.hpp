
/**
 * @file graphicsPipelineBuilder.hpp
 * Declares the vp::GraphicsPipelineBuilder class
*/

#ifndef VP_GRAPHICS_PIPELINE_BUILDER_H
#define VP_GRAPHICS_PIPELINE_BUILDER_H

#include <cstdint>
#include <map>
#include <vector>
#include <vulkan/vulkan.h>

#include "vulkanContext.hpp"
#include "shader/vulkanShader.hpp"
#include "commonEnums.hpp"
#include "log.hpp"

namespace vp {


        // TODO: Refactor this into a class
        struct VulkanGraphicsPipeline {
                VkPipeline              handle = VK_NULL_HANDLE;        ///< Handle to the vulkan pipeline object
                VkPipelineLayout        layoutHandle = VK_NULL_HANDLE;  ///< Handle to the vulkan object that describes the resources used by the pipeline
        };


        /**
         * @brief GraphicsPipelineBuilder
         * Factory class for graphics pipelines
        */
        class GraphicsPipelineBuilder {
        public:
                                                GraphicsPipelineBuilder();
                                                ~GraphicsPipelineBuilder() = default;

                                                // Disable copies
                                                GraphicsPipelineBuilder(const GraphicsPipelineBuilder& other) = delete;
                                                GraphicsPipelineBuilder operator =(const GraphicsPipelineBuilder& other) = delete;

                GraphicsPipelineBuilder&        setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
                GraphicsPipelineBuilder&        setInputAssembly(PrimitiveTopologyType primitiveType);

                GraphicsPipelineBuilder&        setVertexShader(const VulkanShader& shader);
                GraphicsPipelineBuilder&        setFragmentShader(const VulkanShader& shader);

                GraphicsPipelineBuilder&        setRasterizer(bool wireframe, TriangleFrontFace frontFace, CullMode cullMode);
                //GraphicsPipelineBuilder&      setMultisampling(); TODO: Add implementation...
                //GraphicsPipelineBuilder&      setColorBlending(); TODO: Add implementation...

                GraphicsPipelineBuilder&        setDepthTest(CompareOperatorType compareOp);
                //GraphicsPipelineBuilder&      setStencilTest(); TODO: Add implementation...

                bool                            createPipeline(VulkanContext& context, VulkanGraphicsPipeline& pipeline, VkRenderPass& renderPass);
                static void                     destroyPipeline(VulkanContext& context, VulkanGraphicsPipeline& pipeline);
                
                void                            reset();

        private:
                void                            addShaderStageInfo(const VulkanShader& shader);


                bool                                                    m_isViewportSet = false;        ///< Indicates if the setViewport method has been called or not
                VkViewport                                              m_viewport;                     ///< Struct that describes the viewport
                VkRect2D                                                m_scissorRect;                  ///< Struct that describes the scissor region
                
                VkPipelineInputAssemblyStateCreateInfo                  m_inputAssemblyInfo;            ///< Struct that describes the input assembly state

                std::map<ShaderType, VkPipelineShaderStageCreateInfo>   m_shaderStagesInfo;             ///< Structs that describes the programmable shader stages

                std::vector<VkVertexInputBindingDescription>            m_vrtxShaderBindingsInfo;       ///< Structs wich describes the vertex bindings necessary for the vertex shader
                std::vector<VkVertexInputAttributeDescription>          m_vrtxShaderAttributesInfo;     ///< Structs wich describes the vertex attributes taken as input by the vertex shader

                VkPipelineRasterizationStateCreateInfo                  m_rasterizerInfo;               ///< Struct that describes how rasterization of graphic primitives shall be performed
                VkPipelineMultisampleStateCreateInfo                    m_multisamplingInfo;            ///< Struct that describes if multisampling is enabled and how it shall be performed
        
                VkPipelineDepthStencilStateCreateInfo                   m_depthStencilTestInfo;         ///< Struct that describes if depth and/or stencil tests are enabled and how they shall be performed
        
                VkPipelineColorBlendStateCreateInfo                     m_colorBlendingInfo;            ///< Struct that describes if color blending is enabled and how it shall be performed
                std::vector<VkPipelineColorBlendAttachmentState>        m_colorBlendAttachments;        ///< Structs that describes how color blending shall be performed on color attachments
        };

}

#endif // VP_GRAPHICS_PIPELINE_BUILDER_H



/**
 * @file graphicsPipelineBuilder.cpp
 * Implements the vp::GraphicsPipelineBuilder class
*/

#include "graphicsPipelineBuilder.hpp"

namespace vp {


        /**
         * @brief GraphicsPipelineBuilder::GraphicsPipelineBuilder
         * Class constructor, creates a builder with default settings (see @link GraphicsPipelineBuilder::reset)
        */
        GraphicsPipelineBuilder::GraphicsPipelineBuilder()
        {
                reset();
        }


        /**
         * @brief GraphicsPipelineBuilder::setViewport
         * Sets the viewport (region of the framebuffer to which pixels produced by the pipeline can be written)
         * @param x X coordinate of the top left corner of the viewport
         * @param y Y coordinate of the top left corner of the viewport
         * @param width Width of the viewport
         * @param height Height of the viewport
         * @return Reference to this builder instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineBuilder& GraphicsPipelineBuilder::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
                // TODO: Support multiple viewports (and make scissor region configurable???)

                m_isViewportSet = true;

                m_viewport.x                    = static_cast<float>(x);
                m_viewport.y                    = static_cast<float>(y);
                m_viewport.width                = static_cast<float>(width);
                m_viewport.height               = static_cast<float>(height);
                m_viewport.minDepth             = 0.0f;
                m_viewport.maxDepth             = 1.0f;

                m_scissorRect.offset.x          = x;
                m_scissorRect.offset.y          = y;
                m_scissorRect.extent.width      = width;
                m_scissorRect.extent.height     = height;

                return *this;
        }


        /**
         * @brief GraphicsPipelineBuilder::setInputAssembly
         * Sets the type of primitive topology that will be rendered by the pipeline
         * @param primitiveType Primitive topology type that will be rendered by the pipeline
         * @return Reference to this builder instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineBuilder& GraphicsPipelineBuilder::setInputAssembly(PrimitiveTopologyType primitiveType)
        {
                EnumTranslator::translate( primitiveType, m_inputAssemblyInfo.topology );
                return *this;
        }

        /**
         * @brief GraphicsPipelineBuilder::setVertexShader
         * Sets the vertex shader that shall be used by the pipeline
         * @param shader Shader to be used, such shader must remain valid until "createPipeline" is invoked
         * otherwise building of the pipeline will fail
         * @return Reference to this builder instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineBuilder& GraphicsPipelineBuilder::setVertexShader(const VulkanShader& shader)
        {
                if( !shader.isValid() || shader.getType() != ShaderType::VERTEX_SHADER )
                {
                        LOG_ERROR("GraphicsPipelineBuilder::setVertexShader() failed: given shader is invalid or its not a vertex shader!");
                        return *this;
                }

                addShaderStageInfo(shader);

                // Save vertex shader's input bindings and attributes
                m_vrtxShaderBindingsInfo = shader.getVertexBindingsDescriptions();
                m_vrtxShaderAttributesInfo = shader.getVertexAttributesDescriptions();

                return *this;
        }


        /**
         * @brief GraphicsPipelineBuilder::setFragmentShader
         * Sets the vertex shader that shall be used by the pipeline
         * @param shader Shader to be used, such shader must remain valid until "createPipeline" is invoked
         * otherwise building of the pipeline will fail
         * @return Reference to this builder instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineBuilder& GraphicsPipelineBuilder::setFragmentShader(const VulkanShader& shader)
        {
                if( !shader.isValid() || shader.getType() != ShaderType::FRAGMENT_SHADER )
                {
                        LOG_ERROR("GraphicsPipelineBuilder::setFragmentShader() failed: given shader is invalid or its not a fragment shader!");
                        return *this;
                }

                addShaderStageInfo(shader);
                return *this;
        }


        /**
         * @brief GraphicsPipelineBuilder::setRasterizer
         * Sets properties for the rasterization stage of the pipeline
         * @param frontFace Defines which face of a triangle shall be considered the front face 
         * @param cullMode Triangle facing direction used for primitive culling
         * @return Reference to this builder instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineBuilder& GraphicsPipelineBuilder::setRasterizer(bool wireframe, TriangleFrontFace frontFace, CullMode cullMode)
        {
                VkCullModeFlagBits translatedCullMode;
                EnumTranslator::translate( cullMode, translatedCullMode );

                m_rasterizerInfo.polygonMode    = (wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL);
                m_rasterizerInfo.cullMode       = 0 | translatedCullMode;

                EnumTranslator::translate( frontFace, m_rasterizerInfo.frontFace );

                return *this;
        }


        /**
         * @brief GraphicsPipelineBuilder::setDepthTest
         * Enables depth test and sets its properties
         * @param compareOp Specifies the comparison operator to be used for the depth test
         * @return Reference to this builder instance, so that calls to methods can be concatenated
        */
        GraphicsPipelineBuilder& GraphicsPipelineBuilder::setDepthTest(CompareOperatorType compareOp)
        {
                m_depthStencilTestInfo.depthTestEnable          = VK_TRUE;
                m_depthStencilTestInfo.depthWriteEnable         = VK_TRUE;

                EnumTranslator::translate( compareOp, m_depthStencilTestInfo.depthCompareOp );

                return *this;
        }


        /**
         * @brief GraphicsPipelineBuilder::createPipeline
         * Tries to create a graphics pipeline using the properties currently set on the builder
         * @param context Vulkan context for which the pipeline shall be created
         * @param pipeline Output variable in which data for the created pipeline will be stored
         * @param renderPass Render pas for which the pipeline will be built (TODO: decide how to handle render passes)
         * @return True on success, false on failure
        */
        bool GraphicsPipelineBuilder::createPipeline(VulkanContext& context, VulkanGraphicsPipeline& pipeline, VkRenderPass& renderPass)
        {
                if(renderPass == VK_NULL_HANDLE)
                {
                        LOG_ERROR("GraphicsPipelineBuilder::createPipeline() failed: an invalid render pass is given!");
                        return false;
                }

                if(pipeline.handle != VK_NULL_HANDLE || pipeline.layoutHandle != VK_NULL_HANDLE)
                {
                        LOG_ERROR("GraphicsPipelineBuilder::createPipeline() failed: given output variable for the pipeline is already set!");
                        return false;
                }

                // Check that the viewport is set
                if( !m_isViewportSet )
                {
                        LOG_ERROR("GraphicsPipelineBuilder::createPipeline() failed: vieport is not set!");
                        return false;
                }

                auto vrtxShaderIt = m_shaderStagesInfo.find(ShaderType::VERTEX_SHADER);
                auto fragShaderIt = m_shaderStagesInfo.find(ShaderType::FRAGMENT_SHADER);

                // Check that vertex and fragment shaders are set
                if(vrtxShaderIt == m_shaderStagesInfo.end() || fragShaderIt == m_shaderStagesInfo.end())
                {
                        LOG_ERROR("GraphicsPipelineBuilder::createPipeline() failed: vertex/fragment shader is not set!");
                        return false;
                }

                // ====================[ Viewport and scissor region description (fixed function stage) ]====================
                // The viewport struct describes the dimensions of the viewport (the viewport defines how a rendered image is mapped onto the framebuffer).
                // The Scissor struct instead describes the region of the framebuffer to which pixels can be written
                VkPipelineViewportStateCreateInfo viewportInfo {};

                viewportInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportInfo.viewportCount      = 1;
                viewportInfo.pViewports         = &m_viewport;
                viewportInfo.scissorCount       = 1;
                viewportInfo.pScissors          = &m_scissorRect;

                // ====================[ Vertex input description (fixed function stage) ]====================
                // The scope of this section is to describe to Vulkan the format of the vertices that will be passed as input to the pipeline.
                // To do so we need:
                //      - the vertex binding points (vertex buffers will be bound to those "binding points")
                //      - the vertex attributes info: details about each attribute taken as input by the vertex shader

                // Combine binding point descriptions and attributes descriptions
                // This struct describes how vertices are passed as input to the vertex shader, this is similar to glVertexAttribPointer() call in OpenGL API
                VkPipelineVertexInputStateCreateInfo vertexInputInfo {};

                vertexInputInfo.sType                                   = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputInfo.vertexBindingDescriptionCount           = m_vrtxShaderBindingsInfo.size();
                vertexInputInfo.pVertexBindingDescriptions              = m_vrtxShaderBindingsInfo.data();
                vertexInputInfo.vertexAttributeDescriptionCount         = m_vrtxShaderAttributesInfo.size();
                vertexInputInfo.pVertexAttributeDescriptions            = m_vrtxShaderAttributesInfo.data();
                
                // ====================[ Dynamic state description ]====================
                // This struct describes the portions of the pipeline state that can be changed dynamically (without recreating the entire pipeline, before a draw call is issued).
                // This is a Vulkan specific thing, it does not map to a specific graphics pipeline stage (conceptually)
                VkDynamicState dynamicStates[1] = { VK_DYNAMIC_STATE_VIEWPORT };

                VkPipelineDynamicStateCreateInfo dynamicStateInfo {};

                dynamicStateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicStateInfo.dynamicStateCount      = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
                dynamicStateInfo.pDynamicStates         = dynamicStates;

                // ====================[ Programmable shader stages info ]====================
                // Put programmable stages info into a contiguous array
                std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfoArray;

                for(auto& pair : m_shaderStagesInfo)
                        shaderStagesInfoArray.push_back(pair.second);

                // ====================[ Pipeline layout definition ]====================
                // The VkPipelineLayout object describes the resources that will be accessed by the programmable stages of a graphics pipeline.
                // Such resources are described using "descriptor set layout" structs

                VkPipelineLayoutCreateInfo pipelineLayoutInfo {};

                pipelineLayoutInfo.sType                        = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.setLayoutCount               = 0;
                pipelineLayoutInfo.pSetLayouts                  = nullptr;
                pipelineLayoutInfo.pushConstantRangeCount       = 0;
                pipelineLayoutInfo.pPushConstantRanges          = nullptr;

                if( vkCreatePipelineLayout(context.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipeline.layoutHandle) != VK_SUCCESS )
                {
                        LOG_ERROR("GraphicsPipelineBuilder::createPipeline() failed: cannot create pipeline layout, vkCreatePipelineLayout() failed!");
                        return false;
                }

                // ====================[ Pipeline creation ]====================
                // Now that all properties of the programmable and fixed stages of the pipeline are defined, we can finally create the graphics pipeline
                VkGraphicsPipelineCreateInfo pipelineInfo {};

                pipelineInfo.sType                      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipelineInfo.stageCount                 = shaderStagesInfoArray.size();
                pipelineInfo.pStages                    = shaderStagesInfoArray.data();
                pipelineInfo.pVertexInputState          = &vertexInputInfo;
                pipelineInfo.pInputAssemblyState        = &m_inputAssemblyInfo;
                pipelineInfo.pViewportState             = &viewportInfo;
                pipelineInfo.pRasterizationState        = &m_rasterizerInfo;
                pipelineInfo.pMultisampleState          = &m_multisamplingInfo;
                pipelineInfo.pDepthStencilState         = &m_depthStencilTestInfo; 
                pipelineInfo.pColorBlendState           = &m_colorBlendingInfo;
                pipelineInfo.pDynamicState              = &dynamicStateInfo;
                pipelineInfo.layout                     = pipeline.layoutHandle;
                pipelineInfo.renderPass                 = renderPass;
                pipelineInfo.subpass                    = 0;
                pipelineInfo.basePipelineHandle         = VK_NULL_HANDLE;
                pipelineInfo.basePipelineIndex          = -1;
                
                if( vkCreateGraphicsPipelines(context.getLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.handle) != VK_SUCCESS )
                {
                        LOG_ERROR("GraphicsPipelineBuilder::createPipeline() failed: cannot create pipeline, vkCreateGraphicsPipeline() failed!");

                        destroyPipeline(context, pipeline);
                        return false;
                }

                return true;
        }


        /**
         * @brief GraphicsPipelineBuilder::destroyPipeline
         * Utility function used to destroy a graphics pipeline and its layout
         * @param context Vulkan context to which the pipeline is associated
         * @param pipeline The pipeline to be detroyed
        */
        void GraphicsPipelineBuilder::destroyPipeline(VulkanContext& context, VulkanGraphicsPipeline& pipeline)
        {
                if(pipeline.handle != VK_NULL_HANDLE)
                {
                        vkDestroyPipeline(context.getLogicalDevice(), pipeline.handle, nullptr);
                        pipeline.handle = VK_NULL_HANDLE;
                }

                if(pipeline.layoutHandle != VK_NULL_HANDLE)
                {
                        vkDestroyPipelineLayout(context.getLogicalDevice(), pipeline.layoutHandle, nullptr);
                        pipeline.layoutHandle = VK_NULL_HANDLE;
                }
        }



        /**
         * @brief GraphicsPipelineBuilder::reset
         * Resets the builder to the default configuration.
         * By default the builder is setup to create a pipeline with:
         *      - filled triangles primitive topology
         *      - disabled depth and stencil tests
         *      - disabled color blending
         *      - clockwise triangle front face
         *      - disabled culling
         *      - disabled multisampling (sample count equal to 1)
         *
         * To build a valid pipeline the user must define at least:
         *      - a vertex shader
         *      - a fragment shader
         *      - viewport dimensions
        */
        void GraphicsPipelineBuilder::reset()
        {
                // Reset viewport and scissor infos
                m_isViewportSet         = false;
                m_viewport              = {};
                m_scissorRect           = {};

                // Reset input assembly info
                m_inputAssemblyInfo                             = {};
                m_inputAssemblyInfo.sType                       = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                m_inputAssemblyInfo.topology                    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                m_inputAssemblyInfo.primitiveRestartEnable      = VK_FALSE;

                // Reset programmable shader stages infos
                m_shaderStagesInfo.clear();

                // Reset rasterization info
                m_rasterizerInfo                                = {};
                m_rasterizerInfo.sType                          = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                m_rasterizerInfo.depthClampEnable               = VK_FALSE;
                m_rasterizerInfo.rasterizerDiscardEnable        = VK_FALSE;
                m_rasterizerInfo.polygonMode                    = VK_POLYGON_MODE_FILL;
                m_rasterizerInfo.cullMode                       = VK_CULL_MODE_NONE;
                m_rasterizerInfo.frontFace                      = VK_FRONT_FACE_CLOCKWISE;
                m_rasterizerInfo.depthBiasEnable                = VK_FALSE;
                m_rasterizerInfo.lineWidth                      = 1.0f;

                // Reset multisampling info
                m_multisamplingInfo                             = {};
                m_multisamplingInfo.sType                       = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                m_multisamplingInfo.sampleShadingEnable         = VK_FALSE;
                m_multisamplingInfo.rasterizationSamples        = VK_SAMPLE_COUNT_1_BIT;
                m_multisamplingInfo.minSampleShading            = 1.0f;
                m_multisamplingInfo.pSampleMask                 = nullptr;
                m_multisamplingInfo.alphaToCoverageEnable       = VK_FALSE;
                m_multisamplingInfo.alphaToOneEnable            = VK_FALSE;

                // Reset depth and stencil tests info
                m_depthStencilTestInfo                          = {};
                m_depthStencilTestInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                m_depthStencilTestInfo.depthTestEnable          = VK_FALSE;
                m_depthStencilTestInfo.depthWriteEnable         = VK_FALSE;
                m_depthStencilTestInfo.depthCompareOp           = VK_COMPARE_OP_NEVER;

                // Set default color blending properties for the color attachment
                VkPipelineColorBlendAttachmentState colorBlendAttachment {};

                colorBlendAttachment.colorWriteMask             = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                colorBlendAttachment.blendEnable                = VK_FALSE;
                colorBlendAttachment.srcColorBlendFactor        = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstColorBlendFactor        = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.colorBlendOp               = VK_BLEND_OP_ADD;
                colorBlendAttachment.srcAlphaBlendFactor        = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstAlphaBlendFactor        = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.alphaBlendOp               = VK_BLEND_OP_ADD;

                m_colorBlendAttachments.clear();
                m_colorBlendAttachments.push_back(colorBlendAttachment);

                // Reset color blending info
                m_colorBlendingInfo                             = {};
                m_colorBlendingInfo.sType                       = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                m_colorBlendingInfo.logicOpEnable               = VK_FALSE;
                m_colorBlendingInfo.logicOp                     = VK_LOGIC_OP_COPY;
                m_colorBlendingInfo.attachmentCount             = 1;
                m_colorBlendingInfo.pAttachments                = m_colorBlendAttachments.data();
        }


        /**
         * @brief GraphicsPipelineBuilder::addShaderStageInfo
         * @param shader Shader for which 
         * Adds info about a programmable shader stage to the builder
        */
        void GraphicsPipelineBuilder::addShaderStageInfo(const VulkanShader& shader)
        {
                // Create and populate the info struct
                VkPipelineShaderStageCreateInfo shaderStageInfo = {};

                shaderStageInfo.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                shaderStageInfo.module  = shader.getModule();
                shaderStageInfo.pName   = shader.getEntryPoint().c_str();

                EnumTranslator::translate( shader.getType(), shaderStageInfo.stage );

                // Add info struct to the map
                m_shaderStagesInfo.insert( { shader.getType(), shaderStageInfo } );
        }

}

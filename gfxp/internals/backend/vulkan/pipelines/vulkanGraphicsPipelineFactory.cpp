
/**
 * @file vulkanGraphicsPipelineFactory.cpp
 * Implements the gfxp::backend::VulkanGraphicsPipelineFactory class
*/

#include "vulkanGraphicsPipelineFactory.hpp"

namespace gfxp::backend {


        /**
         * @brief VulkanGraphicsPipelineFactory::createPipeline
         * Tries to create a valid VulkanGraphicsPipeline object according to the given pipeline description
         * @param pipelineDesc Struct that describes the properties of the graphics pipeline
         * @param renderPass Render pas for which the pipeline will be built (TODO: decide how to handle render passes)
         * @return Pointer to a valid VulkanGraphicsPipeline object on success, nullptr otherwise
        */
        VulkanGraphicsPipeline* VulkanGraphicsPipelineFactory::createPipeline(const gfxp::GraphicsPipelineDescription& pipelineDesc, VkRenderPass renderPass)
        {
                if( !m_device.isInit() )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::createPipeline() failed: VulkanDevice is not initialized!");
                        return nullptr;
                }

                if(renderPass == VK_NULL_HANDLE)
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::createPipeline() failed: given render pass is invalid!");
                        return nullptr;
                }

                #define TRY_TO(x) if( !x ) \
                        return nullptr;

                // Reset factory properties
                reset();

                // Try to extract all properties necessary to create the graphics pipeline
                TRY_TO( fillViewportInfo(pipelineDesc) )
                TRY_TO( fillInputAssemblyInfo(pipelineDesc.getPrimitiveTopologyType()) )

                TRY_TO( addShaderStageInfo(pipelineDesc.getVertexShader()) )
                TRY_TO( addShaderStageInfo(pipelineDesc.getFragmentShader()) )

                TRY_TO( fillRasterizationStageInfo(pipelineDesc.getRasterizerDescription() ) )
                TRY_TO( fillMultisamplingInfo(pipelineDesc) )
                TRY_TO( fillColorBlendingInfo(pipelineDesc) )
                TRY_TO( fillDepthTestInfo(pipelineDesc.getDepthTestDescription()) )

                VkPipeline pipelineHandle = VK_NULL_HANDLE;
                VkPipelineLayout pipelineLayoutHandle = VK_NULL_HANDLE;

                // ====================[ Describe pipeline dynamic states ]====================
                // This struct describes the portions of the pipeline state that can be changed dynamically (without recreating the entire pipeline, before a draw call is issued).
                // This is a Vulkan specific thing, it does not map to a specific graphics pipeline stage (conceptually)
                VkDynamicState dynamicStates[1] = { VK_DYNAMIC_STATE_VIEWPORT };

                VkPipelineDynamicStateCreateInfo dynamicStateInfo {};

                dynamicStateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicStateInfo.dynamicStateCount      = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
                dynamicStateInfo.pDynamicStates         = dynamicStates;

                // ====================[ Pipeline layout definition ]====================
                // The VkPipelineLayout object describes the resources that will be accessed by the programmable stages of a graphics pipeline.
                // Such resources are described using "descriptor set layout" structs
                VkPipelineLayoutCreateInfo pipelineLayoutInfo {};

                pipelineLayoutInfo.sType                        = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.setLayoutCount               = 0;
                pipelineLayoutInfo.pSetLayouts                  = nullptr;
                pipelineLayoutInfo.pushConstantRangeCount       = 0;
                pipelineLayoutInfo.pPushConstantRanges          = nullptr;

                if( vkCreatePipelineLayout(m_device.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayoutHandle) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::createPipeline() failed: cannot create pipeline layout, call to vkCreatePipelineLayout() failed!");
                        return {};
                }

                // ====================[ Pipeline creation ]====================
                // Now that all properties of the programmable and fixed stages of the pipeline are defined, we can finally create the graphics pipeline
                VkGraphicsPipelineCreateInfo pipelineInfo {};

                pipelineInfo.sType                      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipelineInfo.stageCount                 = m_shaderStagesInfo.size();
                pipelineInfo.pStages                    = m_shaderStagesInfo.data();
                pipelineInfo.pVertexInputState          = &m_vertexInputInfo;
                pipelineInfo.pInputAssemblyState        = &m_inputAssemblyInfo;
                pipelineInfo.pViewportState             = &m_viewportInfo;
                pipelineInfo.pRasterizationState        = &m_rasterizerInfo;
                pipelineInfo.pMultisampleState          = &m_multisamplingInfo;
                pipelineInfo.pDepthStencilState         = &m_depthStencilTestInfo; 
                pipelineInfo.pColorBlendState           = &m_colorBlendingInfo;
                pipelineInfo.pDynamicState              = &dynamicStateInfo;
                pipelineInfo.layout                     = pipelineLayoutHandle;
                pipelineInfo.renderPass                 = renderPass;
                pipelineInfo.subpass                    = 0;
                pipelineInfo.basePipelineHandle         = VK_NULL_HANDLE;
                pipelineInfo.basePipelineIndex          = -1;
                
                if( vkCreateGraphicsPipelines(m_device.getLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelineHandle) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::createPipeline() failed: cannot create pipeline, call to vkCreateGraphicsPipeline() failed!");

                        vkDestroyPipelineLayout(m_device.getLogicalDevice(), pipelineLayoutHandle, nullptr);
                        return {};
                }

                // Create the VulkanGraphicsPipeline object
                VulkanGraphicsPipeline* pipeline = new VulkanGraphicsPipeline(m_device);

                pipeline->handle = pipelineHandle;
                pipeline->layoutHandle = pipelineLayoutHandle;

                return pipeline;
        }


        /**
         * @brief VulkanGraphicsPipelineFactory::destroyPipeline
         * Destroys and deallocates the given VulkanGraphicsPipeline object
         * @param pipeline VulkanGraphicsPipeline object that must be destroyed
        */
        void VulkanGraphicsPipelineFactory::destroyPipeline(VulkanGraphicsPipeline*& pipeline)
        {
                if(pipeline == nullptr)
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::destroyPipeline() failed: pipeline is not valid!");
                        return;
                }

                if( !pipeline->device.isInit() )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::destroyPipeline() failed: VulkanDevice is not initialized!");
                        return;
                }

                if(pipeline->isValid())
                {
                        vkDestroyPipeline(pipeline->device.getLogicalDevice(), pipeline->handle, nullptr);
                        vkDestroyPipelineLayout(pipeline->device.getLogicalDevice(), pipeline->layoutHandle, nullptr);
                }

                delete pipeline;
                pipeline = nullptr;
        }


        /**
         * @brief VulkanGraphicsPipelineFactory::fillViewportInfo
         * Sets viewport and scissor region that shall be used by the factory according to the given description
         * @param viewportDesc API agnostic description from which data shall be extracted
         * @return True on success, false otherwise
        */
        bool VulkanGraphicsPipelineFactory::fillViewportInfo(const gfxp::GraphicsPipelineDescription& pipelineDesc)
        {
                if( !pipelineDesc.isViewportSet() )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::fillViewportinfo() failed: viewport is not in the pipeline description!");
                        return false;
                }

                // TODO: Support multiple viewports (we may have multiple render targets) (and make scissor region configurable???)

                const gfxp::GraphicsPipelineDescription::ViewportDescription& viewportDesc = pipelineDesc.getViewportDescription();

                // ====================[ Viewport and scissor region description (fixed function stage) ]====================
                // The viewport struct describes the dimensions of the viewport (the viewport defines how a rendered image is mapped onto the framebuffer).
                // The Scissor struct instead describes the region of the framebuffer to which pixels can be written
                // Extract viewport details
                
                m_viewport.x                    = static_cast<float>( viewportDesc.x ),
                m_viewport.y                    = static_cast<float>( viewportDesc.y ),
                m_viewport.width                = static_cast<float>( viewportDesc.width ),
                m_viewport.height               = static_cast<float>( viewportDesc.height ),

                // Extract scissor region details
                m_scissorRect.offset.x          = viewportDesc.x;
                m_scissorRect.offset.y          = viewportDesc.y;
                m_scissorRect.extent.width      = viewportDesc.width;
                m_scissorRect.extent.height     = viewportDesc.height;

                // Fill viewport info struct
                m_viewportInfo.viewportCount    = 1;
                m_viewportInfo.pViewports       = &m_viewport;
                m_viewportInfo.scissorCount     = 1;
                m_viewportInfo.pScissors        = &m_scissorRect;

                return true;
        }


        /**
         * @brief VulkanGraphicsPipelineFactory::fillInputAssemblyInfo
         * Sets the input assembly info that shall be used by the factory according to the given description
         * @param primitiveType Type of primitive topology that shall be rendered by the graphics pipeline
         * @return True on success, false otherwise
        */
        bool VulkanGraphicsPipelineFactory::fillInputAssemblyInfo(const gfxp::PrimitiveTopologyType primitiveType)
        {
                if( !VulkanEnumTranslator::translate( primitiveType, m_inputAssemblyInfo.topology ) )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::fillInputAssemblyInfo() failed: cannot convert PrimitiveTopologyType to Vulkan enum value!");
                        return false;
                }

                return true;
        }


        /**
         * @brief VulkanGraphicsPipelineFactory::addShaderStageInfo
         * Adds an info struct for a programmable shader stage according to the given description
         * @param handle Handle associated to the VulkanShader object from which properties shall be extracted
         * @return True on success, false otherwise
        */
        bool VulkanGraphicsPipelineFactory::addShaderStageInfo(const gfxp::ShaderHandle handle)
        {
                VulkanShader* shader = static_cast<VulkanShader*>(handle);

                if(handle == gfxp::INVALID_HANDLE || !shader->isValid())
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::addShaderStageInfo() failed: shader handle is invalid!");
                        return false;
                }

                // Create and populate a shader stage info struct
                VkPipelineShaderStageCreateInfo shaderStageInfo = {};

                shaderStageInfo.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                shaderStageInfo.module  = shader->handle;
                shaderStageInfo.pName   = shader->entryPointName.c_str();

                if( !VulkanEnumTranslator::translate( shader->type, shaderStageInfo.stage ) )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::addShaderStageInfo() failed: cannot convert convert ShaderType to Vulkan enum value!");
                        return false;
                }

                // If the shader is a vertex shader
                if(shader->type == gfxp::ShaderType::VERTEX_SHADER)
                {
                        // Fill the vertex input info struct
                        m_vertexInputInfo.vertexBindingDescriptionCount         = shader->vrtxBindingsDescriptions.size();
                        m_vertexInputInfo.pVertexBindingDescriptions            = shader->vrtxBindingsDescriptions.data();
                        m_vertexInputInfo.vertexAttributeDescriptionCount       = shader->vrtxAttributesDescriptions.size();
                        m_vertexInputInfo.pVertexAttributeDescriptions          = shader->vrtxAttributesDescriptions.data();
                }

                // Add info struct to the map
                m_shaderStagesInfo.push_back(shaderStageInfo);

                return true;
        }


        /**
         * @brief VulkanGraphicsPipelineFactory::fillRasterizationStageInfo
         * Sets the rasterization stage info that shall be used by the factory according to the given description
         * @param rasterizerDesc API agnostic description from which data shall be extracted
         * @return True on success, false otherwise
        */
        bool VulkanGraphicsPipelineFactory::fillRasterizationStageInfo(const gfxp::GraphicsPipelineDescription::RasterizerDescription& rasterizerDesc)
        {
                VkCullModeFlagBits nativeCullMode;

                if( !VulkanEnumTranslator::translate( rasterizerDesc.cullMode, nativeCullMode ) )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::fillRasterizationStageInfo() failed: cannot convert CullMode to Vulkan enum value!");
                        return false;
                }

                if( !VulkanEnumTranslator::translate( rasterizerDesc.triangleFrontFace, m_rasterizerInfo.frontFace ) )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::fillRasterizationStageInfo() failed: cannot convert TriangleFrontFace to Vulkan enum value!");
                        return false;
                }

                m_rasterizerInfo.polygonMode    = (rasterizerDesc.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL);
                m_rasterizerInfo.cullMode       = 0 | nativeCullMode;

                return true;
        }


        /**
         * @brief VulkanGraphicsPipelineFactory::fillMultisamplingInfo
         * Sets the multisampling info that shall be used by the factory according to the given description
         * @param pipelineDesc API agnostic description from which data shall be extracted
         * @return True on success, false otherwise
        */
        bool VulkanGraphicsPipelineFactory::fillMultisamplingInfo(const gfxp::GraphicsPipelineDescription& pipelineDesc)
        {
                /* TODO: For now the struct gets filled with default values.
                 * Add an implementation that actually uses the pipeline description! */

                (void) pipelineDesc; // Unused parameter

                m_multisamplingInfo                             = {};

                m_multisamplingInfo.sType                       = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                m_multisamplingInfo.sampleShadingEnable         = VK_FALSE;
                m_multisamplingInfo.rasterizationSamples        = VK_SAMPLE_COUNT_1_BIT;
                m_multisamplingInfo.minSampleShading            = 1.0f;
                m_multisamplingInfo.pSampleMask                 = nullptr;
                m_multisamplingInfo.alphaToCoverageEnable       = VK_FALSE;
                m_multisamplingInfo.alphaToOneEnable            = VK_FALSE;

                return true;
        }


        /**
         * @brief VulkanGraphicsPipelineFactory::fillColorBlendingInfo
         * Sets the color blending info that shall be used by the factory according to the given description
         * @param pipelineDesc API agnostic description from which data shall be extracted
         * @return True on success, false otherwise
        */
        bool VulkanGraphicsPipelineFactory::fillColorBlendingInfo(const gfxp::GraphicsPipelineDescription& pipelineDesc)
        {
                /* TODO: For now the struct gets filled with default values.
                 * We need to implement an API (in the API agnostic description struct) to define
                 * the color attachments that the pipeline uses and their blending properties */

                (void) pipelineDesc; // Unused parameter
                
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

                // Set default values for the color blending info struct
                m_colorBlendingInfo                             = {};
                m_colorBlendingInfo.sType                       = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                m_colorBlendingInfo.logicOpEnable               = VK_FALSE;
                m_colorBlendingInfo.logicOp                     = VK_LOGIC_OP_COPY;
                m_colorBlendingInfo.attachmentCount             = m_colorBlendAttachments.size();
                m_colorBlendingInfo.pAttachments                = m_colorBlendAttachments.data();

                return true;
        }


        /**
         * @brief VulkanGraphicsPipelineFactory::fillDepthTest
         * Sets the depth test info that shall be used by the factory according to the given description
         * @param depthTestDesc API agnostic description from which data shall be extracted
         * @return True on success, false otherwise
        */
        bool VulkanGraphicsPipelineFactory::fillDepthTestInfo(const gfxp::GraphicsPipelineDescription::DepthTestDescription& depthTestDesc)
        {
                if( !VulkanEnumTranslator::translate(depthTestDesc.compareOp, m_depthStencilTestInfo.depthCompareOp) )
                {
                        LOG_ERROR("VulkanGraphicsPipelineFactory::fillDepthTestInfo() failed: cannot convert CompareOperatorType to Vulkan enum value!");
                        return false;
                }

                m_depthStencilTestInfo.depthTestEnable  = static_cast<VkBool32>(depthTestDesc.enabled);
                m_depthStencilTestInfo.depthWriteEnable = static_cast<VkBool32>(depthTestDesc.enabled);

                return true;
        }


        /**
         * @brief VulkanGraphicsPipelineFactory::reset
         * Resets the factory settings to the default configuration.
         * By default the factory is setup to create a pipeline with:
         *      - filled triangles primitive topology
         *      - disabled depth and stencil tests
         *      - disabled color blending
         *      - clockwise triangle front face
         *      - disabled culling
         *      - disabled multisampling (sample count equal to 1)
         *      - no color attachment
         *
         * To build a valid pipeline the user must define at least:
         *      - a vertex shader
         *      - a fragment shader
         *      - viewport dimensions
         *      - a color attachment
        */
        void VulkanGraphicsPipelineFactory::reset()
        {
                // Reset viewport and scissor infos
                m_viewport              = {};
                m_scissorRect           = {};

                m_viewportInfo          = {};
                m_viewportInfo.sType    = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

                // Reset input assembly info
                m_inputAssemblyInfo                             = {};
                m_inputAssemblyInfo.sType                       = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                m_inputAssemblyInfo.topology                    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                m_inputAssemblyInfo.primitiveRestartEnable      = VK_FALSE;

                // Reset programmable shader stages infos
                m_shaderStagesInfo.clear();

                m_vertexInputInfo                                       = {};
                m_vertexInputInfo.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                m_vertexInputInfo.vertexBindingDescriptionCount         = 0;
                m_vertexInputInfo.pVertexBindingDescriptions            = nullptr;
                m_vertexInputInfo.vertexAttributeDescriptionCount       = 0;
                m_vertexInputInfo.pVertexAttributeDescriptions          = nullptr;

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

                // Reset color blending info
                m_colorBlendingInfo                             = {};
                m_colorBlendingInfo.sType                       = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                m_colorBlendingInfo.logicOpEnable               = VK_FALSE;
                m_colorBlendingInfo.logicOp                     = VK_LOGIC_OP_COPY;
                m_colorBlendingInfo.attachmentCount             = 0;
                m_colorBlendingInfo.pAttachments                = nullptr;
        
                // Reset color attachments blending info
                m_colorBlendAttachments.clear();
        }


} // namespace gfxp::backend

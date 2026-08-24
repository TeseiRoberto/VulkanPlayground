
/**
 * @file vulkanContext.hpp
 * Declares the gfxp::backend::VulkanContext class
*/

#ifndef GFXP_BACKEND_VULKAN_CONTEXT_H
#define GFXP_BACKEND_VULKAN_CONTEXT_H

#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "gfxpLog.hpp"
#include "iGraphicContext.hpp"

#include "internals/gpuResourceHandleManager.hpp"

namespace gfxp::backend {


        // Forward declarations of vulkan GPU resources objects
        struct VulkanBuffer;
        struct VulkanStagingBuffer;
        struct VulkanTexture;
        struct VulkanShader;
        struct VulkanGraphicsPipeline;


        /**
         * @class VulkanContext
         * Implements the gfxp::IGraphicContext interface for the Vulkan API backend.
         * Manages the connection with the Vulkan API driver, keeps track of all GPU resources
         * created through this context and maps them to API agnostic handles
        */
        class VulkanContext : public gfxp::IGraphicContext {
        public:

                /**
                 * @struct QueueFamilyIndices
                 * Utility struct used to keep track of indices to Queue families
                */
                struct QueueFamilyIndices {
                        uint32_t        graphicQueueIndex = UINT32_MAX;         ///< Index to the graphic queue family
                };


                explicit                                VulkanContext() = default;
                                                        ~VulkanContext();

                                                        // Disable copies
                                                        VulkanContext(const VulkanContext& other) = delete;
                                                        VulkanContext(const VulkanContext&& other) = delete;

                                                        VulkanContext& operator =(const VulkanContext& other) = delete;
                                                        VulkanContext& operator =(const VulkanContext&& other) = delete;

                // ======================================================================
                // Context initialization and termination
                virtual bool                            init() override;
                virtual void                            terminate() override;

                virtual bool                            isInit() const override         { return m_instance != VK_NULL_HANDLE; }

                // ======================================================================
                // GPU resources creation
                virtual BufferHandle                    createBuffer(const size_t size, const BufferType type) override;
                virtual TextureHandle                   createTexture(const TextureDescription& imgDesc) override;
                virtual ShaderHandle                    createShader(const ShaderDescription& shaderDesc) override;
                virtual PipelineHandle                  createGraphicsPipeline(const GraphicsPipelineDescription& pipelineDesc) override;

                // ======================================================================
                // GPU resources destruction
                virtual void                            destroyBuffer(BufferHandle& handle) override;
                virtual void                            destroyTexture(TextureHandle& handle) override;
                virtual void                            destroyShader(ShaderHandle& handle) override;
                virtual void                            destroyGraphicsPipeline(PipelineHandle& handle) override;

                // ======================================================================

                inline VkInstance                       getInstance()                   { return m_instance; }
                inline VkPhysicalDevice                 getPhysicalDevice()             { return m_physDevice; }
                inline VkDevice                         getLogicalDevice()              { return m_logicDevice; }
                inline VkQueue                          getGraphicsQueue()              { return m_gfxQueue; }
                inline const QueueFamilyIndices         getQueueFamilyIndices()         { return m_queueFamilyIndices; }

        private:
                bool            createInstance(bool useValidationLayers);
                void            destroyInstance();

                bool            pickPhysicalDevice();

                bool            loadQueueFamilyIndices();
                void            unloadQueueFamilyIndices();

                bool            createLogicalDevice();
                void            destroyLogicalDevice();

                bool            checkValidationLayersSupport();


                VkInstance              m_instance = VK_NULL_HANDLE;                    ///< Vulkan instance associated to the context
                VkPhysicalDevice        m_physDevice = VK_NULL_HANDLE;                  ///< Physical device associated to the context
                VkDevice                m_logicDevice = VK_NULL_HANDLE;                 ///< Logical device associated to the context

                QueueFamilyIndices      m_queueFamilyIndices;                           ///< Indices to queue families required by the context
                VkQueue                 m_gfxQueue = VK_NULL_HANDLE;                    ///< Graphic queue to which renderers using this context submits commands

                // GPU resources managed by the context
                //gfxp::internal::GpuResourceHandleManager<VulkanBuffer*>                 m_bufferResources;              ///< Buffer and staging buffer GPU resources managed by the context
                //gfxp::internal::GpuResourceHandleManager<VulkanTexture*>                m_textureResources;             ///< Texture GPU resources managed by the context
                gfxp::internal::GpuResourceHandleManager<VulkanShader*>                 m_shaderResources;              ///< Shader GPU resources managed by the context
                gfxp::internal::GpuResourceHandleManager<VulkanGraphicsPipeline*>       m_gfxPipelineResources;         ///< Graphics pipeline GPU resources managed by the context


                static const std::vector<const char*>   REQUIRED_VALIDATION_LAYERS;     ///< Validation layers required by the renderer
        };

} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_CONTEXT_H


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
#include "vulkanDevice.hpp"

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
         * Manages the connection with the Vulkan API driver, manages a VulkanDevice, keeps track of all
         * GPU resources created through this context and maps them to API agnostic handles
        */
        class VulkanContext : public gfxp::IGraphicContext {
        public:
                explicit                        VulkanContext() = default;
                                                ~VulkanContext();

                                                // Disable copies
                                                VulkanContext(const VulkanContext& other) = delete;
                                                VulkanContext(const VulkanContext&& other) = delete;

                                                VulkanContext& operator =(const VulkanContext& other) = delete;
                                                VulkanContext& operator =(const VulkanContext&& other) = delete;

                // ======================================================================
                // Context initialization and termination
                virtual bool                    init() override;
                virtual void                    terminate() override;

                virtual bool                    isInit() const override         { return (m_instance != VK_NULL_HANDLE && m_device.isInit()); }

                // ======================================================================
                // GPU resources creation
                virtual BufferHandle            createBuffer(const size_t size, const BufferUsageFlags usage) override;
                virtual TextureHandle           createTexture(const TextureDescription& imgDesc) override;
                virtual ShaderHandle            createShader(const ShaderDescription& shaderDesc) override;
                virtual PipelineHandle          createGraphicsPipeline(const GraphicsPipelineDescription& pipelineDesc) override;

                // ======================================================================
                // GPU resources destruction
                virtual void                    destroyBuffer(BufferHandle& handle) override;
                virtual void                    destroyTexture(TextureHandle& handle) override;
                virtual void                    destroyShader(ShaderHandle& handle) override;
                virtual void                    destroyGraphicsPipeline(PipelineHandle& handle) override;

                // ======================================================================

                inline VkInstance               getInstance()                   { return m_instance; }
                inline VulkanDevice&            getDevice()                     { return m_device; }

        private:
                bool            createInstance(bool useValidationLayers);
                void            destroyInstance();

                bool            checkValidationLayersSupport();


                VkInstance              m_instance = VK_NULL_HANDLE;                    ///< Vulkan instance associated to the context
                VulkanDevice            m_device;                                       ///< Vulkan device associated to the context

                // ======================================================================
                // GPU resources managed by the context
                gfxp::internal::GpuResourceHandleManager<VulkanBuffer*>                 m_bufferResources;              ///< Buffer and staging buffer GPU resources managed by the context
                //gfxp::internal::GpuResourceHandleManager<VulkanTexture*>                m_textureResources;             ///< Texture GPU resources managed by the context
                gfxp::internal::GpuResourceHandleManager<VulkanShader*>                 m_shaderResources;              ///< Shader GPU resources managed by the context
                gfxp::internal::GpuResourceHandleManager<VulkanGraphicsPipeline*>       m_gfxPipelineResources;         ///< Graphics pipeline GPU resources managed by the context


                static const std::vector<const char*>   REQUIRED_VALIDATION_LAYERS;     ///< Validation layers required by gfxp
        };


} // namespace gfxp::backend

#endif // GFXP_BACKEND_VULKAN_CONTEXT_H


/**
 * @file vulkanContext.cpp
 * Implements the gfxp::backend::VulkanContext class
*/

#include "vulkanContext.hpp"

#include "buffers/vulkanBuffer.hpp"
#include "buffers/vulkanBufferFactory.hpp"

//#include "textures/vulkanTexture.hpp"
//#include "textures/vulkanTextureFactory.hpp"

#include "shaders/vulkanShader.hpp"
#include "shaders/vulkanShaderFactory.hpp"

#include "pipelines/vulkanGraphicsPipeline.hpp"
#include "pipelines/vulkanGraphicsPipelineFactory.hpp"

namespace gfxp::backend {


        /**
         * @brief VulkanContext::REQUIRED_VALIDATION_LAYERS
         * Validation layers required by the context
         */
        const std::vector<const char*> VulkanContext::REQUIRED_VALIDATION_LAYERS = {
                "VK_LAYER_KHRONOS_validation"
        };


        /**
         * @brief VulkanContext::~VulkanContext
         * Terminates the vulkan context if not done yet
        */
        VulkanContext::~VulkanContext()
        {
                if(isInit())
                        terminate();
        }


        /**
         * @brief VulkanContext::init
         * Initializes the vulkan context (creates the VkInstance and tries to initialize the device managed by the context)
         * @return true on success, false on failure
         * @note GLFW must be initialized before this method is called!
        */
        bool VulkanContext::init()
        {
                constexpr bool useValidationLayers = true; // TODO: Make this configurable through IGraphicContext

                if(isInit())
                {
                        LOG_WARN("VulkanContext::init() called but context is already initialized!");
                        return false;
                }

                // Try to create the VkInstance
                if( !createInstance(useValidationLayers) )
                        return false;

                // Try to initialize the vulkan device managed by the context
                if( !m_device.init(m_instance) )
                {
                        terminate();
                        return false;
                }

                return true;
        }


        /**
         * @brief VulkanContext::terminate
         * Terminates the vulkan context (destroys all remaining GPU resources and terminates the device managed by the context)
        */
        void VulkanContext::terminate()
        {
                // Utility lambda to correctly destroy all types of buffer resources
                auto bufferDestroyFunc = [this](VulkanBuffer* buffer) {

                                VulkanStagingBuffer* stagingBuffer = dynamic_cast<VulkanStagingBuffer*>(buffer);

                                if(stagingBuffer != nullptr)
                                {
                                        VulkanBufferFactory::destroyStagingBuffer(stagingBuffer);

                                } else {
                                        VulkanBufferFactory::destroyBuffer(buffer);
                                }
                        };

                // Destroy all GPU resources not destroyed yet
                m_bufferResources.applyToAll(bufferDestroyFunc);
                m_bufferResources.clear();

                /* TODO: Object and factory not implemented yet!
                m_textureResources.applyToAll( [this](VulkanTexture* texture) { VulkanTextureFactory::destroyTexture(texture); } );
                m_textureResources.clear();*/

                m_shaderResources.applyToAll( [this](VulkanShader* shader) { VulkanShaderFactory::destroyShader(shader); } );
                m_shaderResources.clear();

                m_gfxPipelineResources.applyToAll( [this](VulkanGraphicsPipeline* pipeline) { VulkanGraphicsPipelineFactory::destroyPipeline(pipeline); } );
                m_gfxPipelineResources.clear();

                // Terminate device and destroy VkInstance
                m_device.terminate();
                destroyInstance();
        }


        /**
         * @brief VulkanContext::createBuffer
         * @param size Size, expressed in bytes, of the buffer to be created
         * @param usage Bitmask that decsribes the ways in which the buffer will be used
         * @return A valid handle (pointer) to a VulkanBuffer object on success, an invalid handle otherwise
        */
        BufferHandle VulkanContext::createBuffer(const size_t size, const BufferUsageFlags usage)
        {
                VulkanBufferFactory bufferFactory(m_device);
                VulkanBuffer* buffer = nullptr;

                // Create a buffer object of the correct type
                if(usage & gfxp::BufferUsage::STAGING_BUFFER)
                {
                        buffer = bufferFactory.createStagingBuffer(size);

                } else {
                        buffer = bufferFactory.createBuffer(size, usage);
                }

                if(buffer == nullptr)
                        return gfxp::INVALID_HANDLE;

                m_bufferResources.add(buffer);
                return static_cast<BufferHandle>(buffer);
        }


        /**
         * @brief VulkanContext::createTexture
         * @param textureDesc Struct that describes the properties of the texture to be created
         * @return A valid handle (pointer) to a VulkanImage object on success, an invalid handle otherwise
        */
        TextureHandle VulkanContext::createTexture(const TextureDescription& textureDesc)
        {
                /* TODO: Implement factory class!
                VulkanTextureFactory textureFactory(m_device);
                VulkanTexture* texture = textureFactory.createTexture(textureDesc);

                if(texture == nullptr)
                        return gfxp::INVALID_HANDLE;

                m_textureResources.add(texture);
                return static_cast<TextureHandle>( texture );*/
                return gfxp::INVALID_HANDLE;
        }


        /**
         * @brief VulkanContext::createShader
         * @param shaderDesc Struct that describes the properties of the shader to be created
         * @return A valid handle (pointer) to a VulkanShader object on success, an invalid handle otherwise
        */
        ShaderHandle VulkanContext::createShader(const ShaderDescription& shaderDesc)
        {
                VulkanShaderFactory shaderFactory(m_device);
                VulkanShader* shader = shaderFactory.createShader(shaderDesc);

                if(shader == nullptr)
                        return gfxp::INVALID_HANDLE;

                m_shaderResources.add(shader);
                return static_cast<ShaderHandle>( shader );
        }


        /**
         * @brief VulkanContext::createGraphicsPipeline
         * @param pipelineDesc Struct that describes the properties of the graphics pipeline to be created
         * @return A valid handle (pointer) to a Vulkan object on success, an invalid handle otherwise
        */
        PipelineHandle VulkanContext::createGraphicsPipeline(const GraphicsPipelineDescription& pipelineDesc)
        {
                VulkanGraphicsPipelineFactory pipelineFactory(m_device);

                // TODO: Decide how to handle render passes
                VulkanGraphicsPipeline* pipeline = pipelineFactory.createPipeline(pipelineDesc, VK_NULL_HANDLE);

                if(pipeline == nullptr)
                        return gfxp::INVALID_HANDLE;

                m_gfxPipelineResources.add(pipeline);
                return static_cast<PipelineHandle>( pipeline );
        }



        /**
         * @brief VulkanContext::destroyBuffer
         * Destoyes the VulkanBuffer object associated to the given handle
         * @param handle Handle to the VulkanBuffer object to be destroyed
        */
        void VulkanContext::destroyBuffer(BufferHandle& handle)
        {
                VulkanBuffer* buffer = static_cast<VulkanBuffer*>(handle);

                // Try to delete the buffer resource
                if( !m_bufferResources.remove(buffer) )
                        return;

                VulkanStagingBuffer* stagingBuffer = dynamic_cast<VulkanStagingBuffer*>(buffer);

                // Destroy the buffer using the appropriate method
                if(stagingBuffer != nullptr)
                {
                        VulkanBufferFactory::destroyStagingBuffer(stagingBuffer);

                } else {
                        VulkanBufferFactory::destroyBuffer(buffer);
                }
        }


        /**
         * @brief VulkanContext::destroyTexture
         * Destoyes the VulkanTexture object associated to the given handle
         * @param handle Handle to the VulkanTexture object to be destroyed
        */
        void VulkanContext::destroyTexture(TextureHandle& handle)
        {
                /* TODO: Implement factory class!
                VulkanTexture* texture = static_cast<VulkanTexture*>(handle);
                
                // Try to delete the texture resource
                if( !m_textureResources.remove(texture) )
                        return;

                VulkanTextureFactory::destroyTexture(texture);*/
        }


        /**
         * @brief VulkanContext::destroyShader
         * Destoyes the VulkanShader object associated to the given handle
         * @param handle Handle to the VulkanShader object to be destroyed
        */
        void VulkanContext::destroyShader(ShaderHandle& handle)
        {
                VulkanShader* shader = static_cast<VulkanShader*>(handle);

                // Try to delete the shader resource
                if( !m_shaderResources.remove(shader) )
                        return;

                VulkanShaderFactory::destroyShader(shader);
        }


        /**
         * @brief VulkanContext::destroyGraphicsPipeline
         * Destoyes the VulkanGraphicsPipeline object associated to the given handle
         * @param handle Handle to the VulkanGraphicsPipeline object to be destroyed
        */
        void VulkanContext::destroyGraphicsPipeline(PipelineHandle& handle)
        {
                VulkanGraphicsPipeline* pipeline = static_cast<VulkanGraphicsPipeline*>(handle);
                
                // Try to delete the graphics pipeline resource
                if( !m_gfxPipelineResources.remove(pipeline) )
                        return;

                VulkanGraphicsPipelineFactory::destroyPipeline(pipeline);
        }


        /**
         * @brief VulkanContext::createInstance
         * Initializes the vulkan instance associated to the context.
         * @param useValidationLayers Indicates if validation layers shall be enabled or not
         * @return True on success, false on failure
         * @note The VkInstance object represents the connection between the application and the Vulkan API (driver)
        */
        bool VulkanContext::createInstance(bool useValidationLayers)
        {
                if( useValidationLayers && !checkValidationLayersSupport() )
                        return false;

                VkApplicationInfo appInfo               = {};
                appInfo.sType                           = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                appInfo.pApplicationName                = "vulkanPlayground";
                appInfo.apiVersion                      = VK_API_VERSION_1_3;

                // Get instance extensions required
                uint32_t extensionCount = 0;
                const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

                if(extensions == NULL)
                {
                        LOG_ERROR("VulkanContext::init() failed: glfwGetRequiredInstanceExtension() returned NULL");
                        return false;
                }

                VkInstanceCreateInfo instanceInfo       = {};
                instanceInfo.sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instanceInfo.pApplicationInfo           = &appInfo;
                instanceInfo.enabledExtensionCount      = extensionCount;
                instanceInfo.ppEnabledExtensionNames    = extensions;

                // Enable validation layers, if necessary
                if(useValidationLayers)
                {
                        instanceInfo.enabledLayerCount = static_cast<uint32_t>( REQUIRED_VALIDATION_LAYERS.size() );
                        instanceInfo.ppEnabledLayerNames = REQUIRED_VALIDATION_LAYERS.data();
                }

                if( vkCreateInstance(&instanceInfo, nullptr, &m_instance) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanContext::init() failed: call to VkCreateInstance() failed");
                        return false;
                }

                return true;
        }


        /**
         * @brief VulkanContext::destroyInstance
         * Destroys the vulkan instance associated to the context
        */
        void VulkanContext::destroyInstance()
        {
                if(m_instance == VK_NULL_HANDLE)
                        return;

                vkDestroyInstance(m_instance, nullptr);
                m_instance = VK_NULL_HANDLE;
        }


        /**
         * @brief VulkanContext::checkValidationLayersSupport
         * Checks if validation layers are supported
         * @return True if all necessary validation layers are supported, false otherwise
        */
        bool VulkanContext::checkValidationLayersSupport()
        {
                // Get number and properties of available validation layers
                uint32_t layerCount = 0;
                vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

                std::vector<VkLayerProperties> availableLayers(layerCount);
                vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

                // Check that all the required layers are available
                for(const char* requiredLayer : REQUIRED_VALIDATION_LAYERS)
                {
                        bool found = false;

                        for(VkLayerProperties& layerProps : availableLayers)
                        {
                                if(strcmp(requiredLayer, layerProps.layerName) == 0)
                                {
                                        found = true;
                                        break;
                                }
                        }

                        if( !found )
                        {
                                LOG_ERROR("VulkanContext::init() failed: validation layer \"%s\" is required but not available!", requiredLayer);
                                return false;
                        }
                }

                return true;
        }


} // namespace gfxp::backend

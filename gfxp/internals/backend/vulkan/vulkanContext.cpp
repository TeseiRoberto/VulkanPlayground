
/**
 * @file vulkanContext.cpp
 * Implements the gfxp::backend::VulkanContext class
*/

#include "vulkanContext.hpp"

//#include "buffers/vulkanBuffer.hpp"
//#include "buffers/vulkanBufferFactory.hpp"
//
//#include "textures/vulkanTexture.hpp"
//#include "textures/vulkanTextureFactory.hpp"

#include "shaders/vulkanShader.hpp"
#include "shaders/vulkanShaderFactory.hpp"

#include "pipelines/vulkanGraphicsPipeline.hpp"
#include "pipelines/vulkanGraphicsPipelineFactory.hpp"

namespace gfxp::backend {


        /**
         * @brief VulkanContext::REQUIRED_VALIDATION_LAYERS
         * Validation layers required by the renderer
         */
        const std::vector<const char*> VulkanContext::REQUIRED_VALIDATION_LAYERS = {
                "VK_LAYER_KHRONOS_validation"
        };


        /**
         * @brief ~VulkanContext::VulkanContext
         * Terminates the vulkan context if not done yet
        */
        VulkanContext::~VulkanContext()
        {
                if(isInit())
                        terminate();
        }


        /**
         * @brief VulkanContext::init
         * Initializes the vulkan context by creating all the basic Vk* objects necessary
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

                #define CHECK(x) if( !x ) { terminate(); return false; }

                if( !createInstance(useValidationLayers) )
                        return false;

                CHECK( pickPhysicalDevice() )
                CHECK( loadQueueFamilyIndices() )
                CHECK( createLogicalDevice() )
        
                return true;
        }


        /**
         * @brief VulkanContext::terminate
         * Terminates the vulkan context and destroys all remaining GPU resources
        */
        void VulkanContext::terminate()
        {
                /* TODO: Objects and factories not implemented yet!
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

                m_textureResources.applyToAll( [this](VulkanTexture* texture) { VulkanTextureFactory::destroyTexture(texture); } );
                m_textureResources.clear();*/

                m_shaderResources.applyToAll( [this](VulkanShader* shader) { VulkanShaderFactory::destroyShader(shader); } );
                m_shaderResources.clear();

                m_gfxPipelineResources.applyToAll( [this](VulkanGraphicsPipeline* pipeline) { VulkanGraphicsPipelineFactory::destroyPipeline(pipeline); } );
                m_gfxPipelineResources.clear();

                // Destroy vulkan context objects
                destroyLogicalDevice();
                unloadQueueFamilyIndices();
                destroyInstance();
        }


        /**
         * @brief VulkanContext::createBuffer
         * @param size Size, expressed in bytes, of the buffer to be created
         * @param type Type of buffer to be created
         * @return A valid handle (pointer) to a VulkanBuffer object on success, an invalid handle otherwise
        */
        BufferHandle VulkanContext::createBuffer(const size_t size, const BufferType type)
        {
                /* TODO: Implement factory class!
                VulkanBufferFactory bufferFactory(*this);
                VulkanBuffer* buffer = nullptr;

                // Create a buffer object of the correct type
                if(type == gfxp::BufferType::STAGING_BUFFER)
                {
                        buffer = bufferFactory.createStagingBuffer(size);

                } else {
                        buffer = bufferFactory.createBuffer(size, type);
                }

                if(buffer == nullptr)
                        return gfxp::INVALID_HANDLE;

                return static_cast<BufferHandle>(buffer);*/
                return gfxp::INVALID_HANDLE;
        }


        /**
         * @brief VulkanContext::createTexture
         * @param textureDesc Struct that describes the properties of the texture to be created
         * @return A valid handle (pointer) to a VulkanImage object on success, an invalid handle otherwise
        */
        TextureHandle VulkanContext::createTexture(const TextureDescription& textureDesc)
        {
                /* TODO: Implement factory class!
                VulkanTextureFactory textureFactory(*this);
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
                VulkanShaderFactory shaderFactory(*this);
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
                VulkanGraphicsPipelineFactory pipelineFactory(*this);

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
                /* TODO: Implement factory class!
                VulkanBuffer* buffer = static_cast<VulkanBuffer*>(handle);

                // Try to delete the buffer resource
                if( !m_bufferResources.remove(handle) )
                        return;

                VulkanStagingBuffer* stagingBuffer = dynamic_cast<VulkanStagingBuffer*>(buffer);

                // Destroy the buffer using the appropriate method
                if(stagingBuffer != nullptr)
                {
                        VulkanBufferFactory::destroyStagingBuffer(stagingBuffer);

                } else {
                        VulkanBufferFactory::destroyBuffer(buffer);
                }*/
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

                VkApplicationInfo appInfo = {};
                appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                appInfo.pApplicationName = "vulkanPlayground";
                appInfo.apiVersion = VK_API_VERSION_1_3;

                // Get instance extensions required
                uint32_t extensionCount = 0;
                const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

                if(extensions == NULL)
                {
                        LOG_ERROR("VulkanContext::init() failed: glfwGetRequiredInstanceExtension() returned NULL");
                        return false;
                }

                VkInstanceCreateInfo instanceInfo = {};
                instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instanceInfo.pApplicationInfo = &appInfo;
                instanceInfo.enabledExtensionCount = extensionCount;
                instanceInfo.ppEnabledExtensionNames = extensions;

                // Enable validation layers, if necessary
                if(useValidationLayers)
                {
                        instanceInfo.enabledLayerCount = static_cast<uint32_t>( REQUIRED_VALIDATION_LAYERS.size() );
                        instanceInfo.ppEnabledLayerNames = REQUIRED_VALIDATION_LAYERS.data();
                }

                if( vkCreateInstance(&instanceInfo, nullptr, &m_instance) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanContext::init() failed: VkCreateInstance() failed");
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

                m_physDevice = VK_NULL_HANDLE;
                m_instance = VK_NULL_HANDLE;
        }


        /**
         * @brief VulkanContext::pickPhysicalDevice
         * Enumerates physical devices and selects one to be associated to the context
         * The physical device is a vulkan object that models a physical GPU supporting vulkan API
         * @return True if a physical device has been chosen, false otherwise
         * @note The physical device gets released when the VkInstance is destroyed, so there is no
         * "releasePhysicalDevice" method in the VulkanContext class.
        */
        bool VulkanContext::pickPhysicalDevice()
        {
                // Strings used to log device type (indexed using the VkPhysicalDeviceType enum)
                static const char* DEVICE_TYPE_STR[] = {
                        "unknown",              // VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
                        "integrated GPU",       // VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
                        "discrete GPU",         // VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
                        "virtual GPU",          // VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
                        "CPU simulated GPU"     // VK_PHYSICAL_DEVICE_TYPE_CPU = 4,
                };

                uint32_t deviceCount = 0;

                if( vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanContext::init() failed: cannot get number of available physical devices, vkEnumeratePhysicalDevices() failed!");
                        return false;
                }

                if(deviceCount == 0)
                {
                        LOG_ERROR("VulkanContext::init() failed: no physical device is available!");
                        return false;
                }

                std::vector<VkPhysicalDevice> devices(deviceCount);
                if( vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data()) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanContext::init() failed: cannot get physical devices, vkEnumeratePhysicalDevices() failed!");
                        return false;
                }

                // Pick the first available device
                m_physDevice = devices[0];

                // Log details about all the available physical devices (just for debug purpouses)
                LOG_INFO("Available physical devices:");
                for(auto& device : devices)
                {
                        VkPhysicalDeviceProperties2 props;
                        props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

                        vkGetPhysicalDeviceProperties2(device, &props);
                        
                        LOG_INFO("Physical device:");
                        LOG_INFO("    - vendor ID: %u",   props.properties.vendorID);
                        LOG_INFO("    - device ID: %u",   props.properties.deviceID);
                        LOG_INFO("    - device name: %s", props.properties.deviceName);
                        LOG_INFO("    - device type: %s", DEVICE_TYPE_STR[props.properties.deviceType]);
                }

                return true;
        }


        /**
         * @brief VulkanContext::loadQueueFamilyIndices
         * Queries queue families supported by the physical device and tries to load indices to
         * the family queues required by the context
         * @return True if the physical device supports the queue families required by the context
         * and indices to them are loaded successfully, false otherwise
        */
        bool VulkanContext::loadQueueFamilyIndices()
        {
                uint32_t queueFamilyCount = 0;

                // Get number of queue families supported by the physical device
                vkGetPhysicalDeviceQueueFamilyProperties(m_physDevice, &queueFamilyCount, nullptr);

                if(queueFamilyCount == 0)
                {
                        LOG_ERROR("VulkanContext::init() failed: physical device, does not support any queue family!");
                        return false;
                }

                std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

                // Get properties of all queue families supported by the physical device
                vkGetPhysicalDeviceQueueFamilyProperties(m_physDevice, &queueFamilyCount, queueFamilies.data());

                // Search for a queue family that support graphics operations
                for(uint32_t i = 0; i < queueFamilyCount; ++i)
                {
                        // Check if current family is a graphic queue family which supports presentation to a window too
                        if( queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                                glfwGetPhysicalDevicePresentationSupport(m_instance, m_physDevice, i) )
                        {
                                m_queueFamilyIndices.graphicQueueIndex = i;
                                break;
                        }
                }

                return (m_queueFamilyIndices.graphicQueueIndex != UINT32_MAX);
        }


        /**
         * @brief VulkanContext::unloadQueueFamilyIndices
         * Resets the queue family indices loaded by the context to dummy values
        */
        void VulkanContext::unloadQueueFamilyIndices()
        {
                m_queueFamilyIndices.graphicQueueIndex = UINT32_MAX;
        }


        /**
         * @brief VulkanContext::createLogicalDevice
         * Creates the logical device associated to the context and retrieves handle to the necessary VkQueues
         * @return True if a logical device has been created successfully, false otherwise
        */
        bool VulkanContext::createLogicalDevice()
        {
                // List of device extensions required by the renderer
                std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

                // Set features to be used (Vulkan 1.0, 1.2 and 1.3 versions)
                VkPhysicalDeviceVulkan12Features enabledVk12Features {};
                enabledVk12Features.sType                                       = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
                enabledVk12Features.descriptorIndexing                          = true;
                enabledVk12Features.shaderSampledImageArrayNonUniformIndexing   = true;
                enabledVk12Features.descriptorBindingVariableDescriptorCount    = true;
                enabledVk12Features.runtimeDescriptorArray                      = true;
                enabledVk12Features.bufferDeviceAddress                         = true;

                VkPhysicalDeviceVulkan13Features enabledVk13Features {};
                enabledVk13Features.sType                                       = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
                enabledVk13Features.pNext                                       = &enabledVk12Features;
                enabledVk13Features.synchronization2                            = true;
                enabledVk13Features.dynamicRendering                            = true;

                VkPhysicalDeviceFeatures enabledVk10Features {};
                enabledVk10Features.samplerAnisotropy                           = true;

                const float queuePriorities[] = { 1.0f };
                
                // Create structs to describe queues required by the context
                VkDeviceQueueCreateInfo queueInfo {};
                queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueInfo.queueFamilyIndex = m_queueFamilyIndices.graphicQueueIndex;
                queueInfo.queueCount = 1;
                queueInfo.pQueuePriorities = queuePriorities;

                // Create struct to describe the logical device to be created
                VkDeviceCreateInfo deviceInfo {};
                deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                deviceInfo.pNext = &enabledVk13Features;
                deviceInfo.queueCreateInfoCount = 1;
                deviceInfo.pQueueCreateInfos = &queueInfo;
                deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
                deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
                deviceInfo.pEnabledFeatures = &enabledVk10Features;

                // Create the logical device
                if( vkCreateDevice(m_physDevice, &deviceInfo, nullptr, &m_logicDevice) != VK_SUCCESS )
                {
                        LOG_ERROR("VulkanContext::init() failed: logical device could not be created, vkCreateDevice() failed");
                        return false;
                }

                // Retrieve handle to the graphic queue
                vkGetDeviceQueue(m_logicDevice, m_queueFamilyIndices.graphicQueueIndex, 0, &m_gfxQueue);

                return true;
        }


        /**
         * @brief VulkanContext::destroyLogicalDevice
         * Destroys the logical device and the queues associated to the context
        */
        void VulkanContext::destroyLogicalDevice()
        {
                if(m_logicDevice == VK_NULL_HANDLE)
                        return;

                vkDestroyDevice(m_logicDevice, nullptr);
                m_logicDevice = VK_NULL_HANDLE;
                m_gfxQueue = VK_NULL_HANDLE;
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

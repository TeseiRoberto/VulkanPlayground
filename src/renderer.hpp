
/**
 * @file renderer.hpp
 * Defines the vp::Renderer class
*/

#ifndef VP_RENDERER_H
#define VP_RENDERER_H

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "log.hpp"

namespace vp {


        class Renderer {
        public:
                explicit        Renderer() = default;
                                ~Renderer();

                bool            init(GLFWwindow* wnd);
                void            terminate();

                inline bool     isInit() const          { return m_instance != VK_NULL_HANDLE; }

        private:

                /**
                 * @struct QueueFamilyIndices
                 * Utility struct used to keep track of indices to Queue families
                */
                struct QueueFamilyIndices {
                        uint32_t        graphicQueueIndex = UINT32_MAX;         ///< Index to the graphic queue family
                };


                bool            createInstance();
                void            destroyInstance();

                bool            pickPhysicalDevice();

                bool            loadQueueFamilyIndices();
                void            unloadQueueFamilyIndices();

                bool            createLogicalDevice();
                void            destroyLogicalDevice();

                bool            createSurface(GLFWwindow* wnd);
                void            destroySurface();


                VkInstance              m_instance = VK_NULL_HANDLE;            ///< Vulkan instance used by the renderer
                VkPhysicalDevice        m_physDevice = VK_NULL_HANDLE;          ///< Physical device used by the renderer
                VkDevice                m_logicDevice = VK_NULL_HANDLE;         ///< Logical device used by the renderer

                QueueFamilyIndices      m_queueFamilyIndices;                   ///< Indices to queue families required by the renderer
                VkQueue                 m_gfxQueue = VK_NULL_HANDLE;            ///< Graphic queue to which the renderer submits commands
        
                VkSurfaceKHR            m_surface = VK_NULL_HANDLE;             ///< Surface on which the render will draw
        };

}

#endif // VP_RENDERER_H

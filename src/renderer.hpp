
/**
 * @file renderer.hpp
 * Defines the vp::Renderer class
*/

#ifndef VP_RENDERER_H
#define VP_RENDERER_H

#include <cstdint>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "log.hpp"

namespace vp {


        class Renderer {
        public:
                explicit        Renderer() = default;
                                ~Renderer();

                bool            init();
                void            terminate();

                inline bool     isInit() const          { return m_instance != VK_NULL_HANDLE; }

        private:
                bool            createInstance();
                void            destroyInstance();


                VkInstance      m_instance = VK_NULL_HANDLE;    ///< Vulkan instance used by the renderer
        };

}

#endif // VP_RENDERER_H

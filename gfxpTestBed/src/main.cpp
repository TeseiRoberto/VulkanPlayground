
/**
 * My attempt at learning Vulkan API while writing a Vulkan renderer
 * following the tutorial: https://www.howtovulkan.com/#intro
*/


#include "gfxp/gfxpLog.hpp"
#include "gfxp/iGraphicContext.hpp"
#include "graphics/renderer.hpp"

#include <GLFW/glfw3.h>

int main()
{
        if( !glfwInit() )
        {
                LOG_ERROR("GLFW initialization failed!");
                return -1;
        }

        // Create window
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* wnd = glfwCreateWindow(720, 480, "Vulkan playground", NULL, NULL);
        if(wnd == nullptr)
        {
                LOG_ERROR("GLFW window creation failed!");
                glfwTerminate();
                return -1;
        }

        glfwShowWindow(wnd);

        // Create graphic context
        std::unique_ptr<gfxp::IGraphicContext> gfxContext = gfxp::IGraphicContext::create(gfxp::GraphicApi::VULKAN);
        if( !gfxContext->init() )
        {
                glfwDestroyWindow(wnd);
                glfwTerminate();
                return -1;
        }

        // Create renderer
        Renderer renderer(gfxContext);
        if( !renderer.init(wnd) )
        {
                gfxContext->terminate();
                glfwDestroyWindow(wnd);
                glfwTerminate();
                return -1;
        }

        // Application main loop
        while( !glfwWindowShouldClose(wnd) )
        {
                renderer.drawFrame();

                glfwSwapBuffers(wnd);
                glfwPollEvents();
        }

        // Clean up
        renderer.terminate();
        gfxContext->terminate();
        glfwDestroyWindow(wnd);
        glfwTerminate();

        return 0;
}

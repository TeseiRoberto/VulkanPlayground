
/**
 * My attempt at learning Vulkan API while writing a Vulkan renderer
 * following the tutorial: https://www.howtovulkan.com/#intro
*/


#include "graphics/renderer.hpp"
#include "log.hpp"
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

        // Create renderer
        vp::Renderer renderer;
        if( !renderer.init(wnd) )
        {
                glfwDestroyWindow(wnd);
                glfwTerminate();
                return -1;
        }

        while( !glfwWindowShouldClose(wnd) )
        {
                renderer.drawFrame();

                glfwSwapBuffers(wnd);
                glfwPollEvents();
        }

        renderer.terminate();
        glfwDestroyWindow(wnd);
        glfwTerminate();
        return 0;
}

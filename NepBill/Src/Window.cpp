#include <iostream>
#include "GLFW/glfw3.h"
#include "Window.h"

namespace NepBill
{
    int Window::Init(const std::string &Name, const IVec2 &Size)
    {
        // GL 3.0 + GLSL 130
        const char *glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW\n";
            return -1;
        }

        GLFWwindow *window = glfwCreateWindow(
            1280, 720,
            "GLFW Window",
            nullptr,
            nullptr);

        if (!window)
        {
            std::cerr << "Failed to create window\n";
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);
        _Window = window;
        std::cout << "Created Window\n";
        return true;
    }

    void Window::Destroy()
    {
        glfwDestroyWindow(_Window);
        glfwTerminate();
    }

    bool Window::ShouldClose()
    {
        return glfwWindowShouldClose(_Window);
    }
    
    void Window::Update()
    {
        glfwPollEvents();
        glfwSwapBuffers(_Window);
    }
} // namespace NepBill

#include "NepBill.h"
#include <iostream>

#include "GLFW/glfw3.h"
#include "SQLiteCpp/SQLiteCpp.h"

#define PRINT(x)        \
    {                   \
        std::cout << x; \
    }
#define PRINTLN(x)              \
    {                           \
        std::cout << x << "\n"; \
    }

int main(int argc, char const *argv[])
{
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
    PRINTLN("Created Window");

    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Present frame
        glfwSwapBuffers(window);

        // Process events
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    PRINTLN("Destroyed Window");

    return 0;
}

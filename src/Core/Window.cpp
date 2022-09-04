#include "Window.hpp"
#include <stdexcept>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
        printf("Pressed: %c\n", key);
    
    if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        exit(0);
    
    if(key == GLFW_KEY_E && action == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if(key == GLFW_KEY_E && action == GLFW_RELEASE)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

namespace nmGfx
{
    Window::Window(int windowWidth, int windowHeight, int videoWidth, int videoHeight, const char* title, WindowFlags flags)
        : _videoWidth(videoWidth), _videoHeight(videoHeight)
    {
        if(!glfwInit())
            throw std::runtime_error("Failed to initialize glfw");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwSetErrorCallback(error_callback);

        _pWindow = glfwCreateWindow(windowWidth, windowHeight, title, NULL, NULL);
        if(!_pWindow)
            throw std::runtime_error("Failed to initialize window");
        
        glfwMakeContextCurrent(_pWindow);
        gladLoadGL();
        
        glfwSetKeyCallback(_pWindow, key_callback);
        glfwSetFramebufferSizeCallback(_pWindow, framebuffer_size_callback);  

        glfwSwapInterval(1);

        // glfwSetInputMode(_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool Window::ShouldClose()
    {
        if(_pWindow == nullptr)
            return printf("Can't call ShouldClose, Window was null\n") && 0;
        return glfwWindowShouldClose(_pWindow);
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(_pWindow);
    }

    int Window::GetWindowMousePosX()
    {
        double x, y;
        glfwGetCursorPos(_pWindow, &x, &y);
        return (int)x;
    }

    int Window::GetWindowMousePosY()
    {
        double x, y;
        glfwGetCursorPos(_pWindow, &x, &y);
        return (int)y;
    }

    int Window::GetVideoMousePosX()
    {
        double posX = (double)GetWindowMousePosX();
        posX *= (double)GetVideoWidth() / (double)GetWindowWidth();
        return (int)posX;
    }

    int Window::GetVideoMousePosY()
    {
        double posY = (double)GetWindowMousePosY();
        posY *= (double)GetVideoHeight() / (double)GetWindowHeight();
        return (int)posY;
    }
    


    int Window::GetWindowHeight()
    {
        int w, h;
        glfwGetWindowSize(_pWindow, &w, &h);
        return h;
    }

    int Window::GetWindowWidth()
    {
        int w, h;
        glfwGetWindowSize(_pWindow, &w, &h);
        return w;
    }

    void Window::UnbindFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, GetWindowWidth(), GetWindowHeight());
    }



    // void Window::SetClearColor(float r, float g, float b, float a /*= 1.0f*/)
    // {
    //     glClearColor(r, g, b, a);
    // }

    // void Window::Clear(uint32_t bits /*= ClearBit::COLOR*/)
    // {
    //     GLbitfield flags = 0;
    //     if(bits & ClearBit::COLOR)
    //         flags |= GL_COLOR_BUFFER_BIT;
    //     if(bits & ClearBit::DEPTH)
    //         flags |= GL_DEPTH_BUFFER_BIT;
    //     
    //     glClear(flags);
    // };
} // namespace nmGfx

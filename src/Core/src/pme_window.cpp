#include "Core/include/pme_window.h"

pme::Window::Window(int width, int height, std::string name) : width{width}, height{height}, windowName{name}
{
    Initialize();
}

pme::Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void pme::Window::CreateVulkanSurfaceOverGlfw(VkInstance instance, VkSurfaceKHR *surface)
{
    auto result = glfwCreateWindowSurface(instance, window, nullptr, surface);
    if(result != VK_SUCCESS )
        throw std::runtime_error("Failed to craete window surface");

    pme::Logger::Log(LogLevel::Success, "Window surface was created");
}

void pme::Window::FramebufferResizeCallback(GLFWwindow *window, int width, int height) 
{
    auto pwindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    pwindow->framebufferResized = true;
    pwindow->width = width;
    pwindow->height = height;
}

void pme::Window::Initialize()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
}


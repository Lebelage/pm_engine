#pragma once
#include "Utils/include/pme_logger.h"

#include <string>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vulkan/vulkan.h>

namespace pme
{
    class Window
    {
    public:
        Window(int width, int height, std::string name);

        ~Window();

        Window(const Window &) = delete;

        Window &operator=(const Window &) = delete;

        bool ShouldClose() { return glfwWindowShouldClose(window); }

        bool WasWindowResized() const { return framebufferResized; }

        void ResetWindowResizedFlag() { framebufferResized = false; }

        GLFWwindow *GetGLFWwindow() const { return window; }

        VkExtent2D GetWindowExtent() const { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

        void CreateVulkanSurfaceOverGlfw(VkInstance instance, VkSurfaceKHR *surface);

    private:
        static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
        void Initialize();

    private:
        int width;
        int height;
        bool framebufferResized = false;

        std::string windowName;
        GLFWwindow *window{};
    };
}
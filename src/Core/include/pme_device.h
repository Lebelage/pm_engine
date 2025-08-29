#pragma once
#include "Core/include/pme_window.h"
#include "Utils/include/pme_logger.h"
#include <vector>
#include <set>
#include <unordered_set>
#include <string>
#include <iostream>
#include <exception>

namespace pme
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        uint32_t graphicsFamily{};
        uint32_t presentFamily{};

        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;

        bool IsComplete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class PmeDevice
    {
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
    public:
        PmeDevice(Window &window);
        ~PmeDevice();

        PmeDevice(const PmeDevice &) = delete;
        PmeDevice operator=(const PmeDevice &) = delete;

        PmeDevice(PmeDevice &&) = delete;
        PmeDevice &operator=(PmeDevice &&) = delete;

    private:
        void Initialize();
        void CreateInstance();
        void CreateSurface();
        void PickPhysicalDevices();
        void CreateLogicalDevice();
        void CreateCommandPool();

    private:
        std::vector<const char *> GetRequiredExtensions() const;
        void HasGflwRequiredInstanceExtensions();
        bool IsDeviceSuitable(VkPhysicalDevice physicalDevice);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        void Release();

    private:
        bool CheckValidationLayerSupport();
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
        void SetupDebugMessenger();

    public:
        VkDevice GetDevice() { return vkDevice; }
        VkSurfaceKHR GetSurface() { return vkSurface; }

        VkQueue GetGraphicsQueue() { return graphicsQueue; }
        VkQueue GetPresentQueue() { return presentQueue; }

        SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(vkPhysicalDevice); }

        int32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(vkPhysicalDevice); }

        VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    private:
        Window &window;

        VkInstance vkInstance{};
        VkDevice vkDevice{};
        VkSurfaceKHR vkSurface{};
        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkCommandPool vkCommandPool{};

        VkDebugUtilsMessengerEXT debugMessenger{};
        VkPhysicalDeviceProperties vkProperties;

        VkQueue graphicsQueue{};
        VkQueue presentQueue{};

        const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
}
#pragma once
#include "Core/include/pme_device.h"
#include "Utils/include/pme_logger.h"
#include <memory>

namespace pme
{
    class PmeSwapChain
    {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        PmeSwapChain(PmeDevice &device, VkExtent2D windowExtent);
        PmeSwapChain(PmeDevice &device, VkExtent2D windowExtent, std::shared_ptr<PmeSwapChain> previousSwapChain);

        ~PmeSwapChain();

        PmeSwapChain(const PmeSwapChain &) = delete;
        PmeSwapChain &operator=(const PmeSwapChain &) = delete;

    private:
        void Initialize();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateRenderPass();
        void CreateFrameBuffers();
        void CreateDepthResources();
        void CreateSyncObjects();

    private:
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        VkFormat FindDepthFormat();

    private:
        void Release();

    public:
        VkFormat GetSwapChainImageFormat() { return swapChainImageFormat; }

        VkFramebuffer GetFrameBuffer(int index) { return swapChainFramebuffers[index]; }

        VkRenderPass GetRenderPass() { return vkRenderPass; }

        VkImageView GetImageView(int index) { return swapChainImageViews[index]; }

        size_t GetImageCount() { return swapChainImages.size(); }

        VkExtent2D GetSwapChainExtent() { return swapChainExtent; }

        uint32_t GetWidth() const { return swapChainExtent.width; }

        uint32_t GetHeight() const { return swapChainExtent.height; }

    private:
        VkSwapchainKHR vkSwapChain;
        VkRenderPass vkRenderPass{};

        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent{};

        PmeDevice &device;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        VkExtent2D windowExtent;

        std::shared_ptr<PmeSwapChain> oldSwapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    };
}
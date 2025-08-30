#pragma once
#include <exception>
#include <iostream>
#include "Core/include/pme_device.h"
#include "Core/include/pme_swapchain.h"
#include "Core/include/pme_window.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
namespace pme
{
    class PmeRenderer
    {
    public:
        PmeRenderer(Window &window, PmeDevice &device);
        ~PmeRenderer();

        PmeRenderer(const PmeRenderer &) = delete;
        PmeRenderer &operator=(const PmeRenderer &) = delete;

    public:
        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

    public:
        VkRenderPass GetRenderPass() const { return pSwapChain->GetRenderPass(); }
        bool IsFrameStarted() const { return isFrameStarted; }

        VkCommandBuffer TryGetCurrentCommandBuffer() const
        {
            try
            {
                if (isFrameStarted)
                    throw std::runtime_error("Cannot get command buffer when frame not in progress");
            }
            catch (std::runtime_error ex)
            {
            }
            return commandBuffers[currentImageIndex];
        }

    private:
        void RecreateSwapChain();
        void CreateCommandBuffers();
        void FreeCommandBuffers();

    private:
        Window &window;
        PmeDevice &device;
        std::unique_ptr<PmeSwapChain> pSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        bool isFrameStarted;
    };

}
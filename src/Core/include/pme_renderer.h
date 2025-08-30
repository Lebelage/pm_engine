#pragma once
#include <exception>
#include <iostream>
#include "Core/include/pme_device.h"
#include "Core/include/pme_swapchain.h"
#include "Core/include/pme_window.h"

#include <cassert>
#include <memory>
#include <vector>
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
        int GetFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }
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
            return commandBuffers[currentFrameIndex];
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
        int currentFrameIndex;
        bool isFrameStarted;
    };

}
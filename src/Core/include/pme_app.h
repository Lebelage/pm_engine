#pragma once
#include <exception>
#include <iostream>
#include <array>
#include "Core/include/pme_window.h"
#include "Core/include/pme_device.h"
#include "Core/include/pme_swapchain.h"
#include "Core/include/pme_pipeline.h"
namespace pme
{
    class App
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App();
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;

        void Run();
        void DoFrame();

    private:
    void CreatePipeLineLayout();
    void CreatePipeLine();

    void CreateCommandBuffer();
    private:
        VkPipelineLayout pipelineLayout;
        std::unique_ptr<PmePipeline> pPipeline;

        std::vector<VkCommandBuffer> commandBuffers;

        Window window;
        PmeDevice device;
        PmeSwapChain swapChain;
    };

}
#pragma once
#include <exception>
#include <iostream>
#include "Core/include/pme_window.h"
#include "Core/include/pme_device.h"
#include "Core/include/pme_swapchain.h"
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

        private:
        Window window;
        PmeDevice device;
        PmeSwapChain swapChain;
    };

}
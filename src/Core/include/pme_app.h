#pragma once
#include <exception>
#include <iostream>
#include <array>
#include "Core/include/pme_window.h"
#include "Core/include/pme_device.h"
#include "Core/include/pme_objects.h"
#include "Core/include/pme_renderer.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
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
        void LoadObjects();

    private:
        Window window;
        PmeDevice device;
        PmeRenderer renderer;
        std::vector<PmeObject> pmeObjects;
    };

}
#pragma once
#include "Core/include/pme_window.h"
#include "Core/include/pme_device.h"
#include "Core/include/pme_objects.h"
#include "Core/include/pme_renderer.h"
#include "Core/include/pme_camera.h"
#include "Core/include/pme_input_keyborad.h"
#include "Core/include/pme_descriptors.h"

#include <exception>
#include <iostream>
#include <array>
#include <chrono>
#include <glm/gtc/constants.hpp>

namespace pme
{
    struct GlobalUbo
    {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
    };
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
        std::unique_ptr<PmeDescriptorPool> globalPool;
    };

}
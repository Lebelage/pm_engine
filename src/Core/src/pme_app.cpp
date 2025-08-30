#include "Core/include/pme_app.h"

#include "Core/include/pme_rendersystem.h"
pme::App::App() : window{WIDTH, HEIGHT, "PhysicsModelingEngine"}, device{window}, renderer{window, device}
{
    try
    {
        LoadObjects();
    }
    catch (std::runtime_error ex)
    {
    }
}

pme::App::~App()
{
}

void pme::App::Run()
{
    RenderSystem renderSystem{device, renderer.GetRenderPass()};

    while (!window.ShouldClose())
    {
        glfwPollEvents();

        if(auto commandBuffer = renderer.BeginFrame())
        {
            renderer.BeginSwapChainRenderPass(commandBuffer);
            renderSystem.RenderObjects(commandBuffer, pmeObjects);
            renderer.EndSwapChainRenderPass(commandBuffer);
            renderer.EndFrame();
        }
    }
}

void pme::App::LoadObjects()
{
    std::vector<Vertex> verticies{
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.6f}}, {{0.5f, 0.5f}, {0.0f, 0.5f, 0.0f}}, {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    auto pModel = std::make_shared<PmeModel>(device, verticies);
    auto triangle = PmeObject::CreateObject();
    triangle.model = pModel;
    triangle.color = {.1f, .8f, .1f};
    triangle.transform2d.translation.x = .0f;
    triangle.transform2d.scale = {2.f, .5f};
    triangle.transform2d.rotation = .25f * glm::two_pi<float>();

    pmeObjects.push_back(std::move(triangle));
}



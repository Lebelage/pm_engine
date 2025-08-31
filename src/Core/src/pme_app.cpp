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

    PmeCamera camera{};
    auto viewerObject = PmeObject::CreateObject();
    KeyboradInput cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();
    while (!window.ShouldClose())
    {     
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        cameraController.MoveInPlaneXZ(window.GetGLFWwindow(), frameTime, viewerObject);
        camera.SetViewXYZ(viewerObject.transform.translation, viewerObject.transform.rotation);
        float aspect = renderer.GetAspectRatio();

        camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
        
        if (auto commandBuffer = renderer.BeginFrame())
        {
            renderer.BeginSwapChainRenderPass(commandBuffer);
            renderSystem.RenderObjects(commandBuffer, pmeObjects, camera);
            renderer.EndSwapChainRenderPass(commandBuffer);
            renderer.EndFrame();
        }
    }
}

void pme::App::LoadObjects()
{
    try
    {
        std::shared_ptr<PmeModel> model = PmeModel::CreateModelFromFile(device, "/Users/frame/Projects/CPP_projects/pm_engine/models/smooth_vase.obj");
        auto cube = PmeObject::CreateObject();
        cube.model = model;
        cube.transform.translation = {.0f, .0f, 5.0f};
        cube.transform.scale = {.5f, .5f, .5f};
        // cube.transform.translation = {.0f, .0f, .5f};
        pmeObjects.push_back(std::move(cube));

    } catch(std::runtime_error ex)
    {
        Logger::Log(LogLevel::Error, ex.what());
    }
}

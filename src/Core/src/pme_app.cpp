#include "Core/include/pme_app.h"
#include "Core/include/pme_rendersystem.h"
#include "Core/include/pme_buffer.h"
#include "Core/include/pme_descriptors.h"

pme::App::App() : window{WIDTH, HEIGHT, "PhysicsModelingEngine"}, device{window}, renderer{window, device}
{
    globalPool = PmeDescriptorPool::Builder(device)
                     .SetMaxSets(PmeSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, PmeSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .Build();
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
    std::vector<std::unique_ptr<PmeBuffer>> uboBuffers(PmeSwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < uboBuffers.size(); i++)
    {
        uboBuffers[i] = std::make_unique<PmeBuffer>(
            device,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->Map();
    }

    auto globalSetLayout = PmeDescriptorSetLayout::Builder(device)
                               .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                               .Build();

    std::vector<VkDescriptorSet> globalDescriptorsSets(PmeSwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < globalDescriptorsSets.size(); i++)
    {
        auto bufferInfo = uboBuffers[i]->DescriptorInfo();
        PmeDescriptorWriter(*globalSetLayout, *globalPool)
            .WriteBuffer(0, &bufferInfo)
            .Build(globalDescriptorsSets[i]);
    }

    RenderSystem renderSystem{device, renderer.GetRenderPass(), globalSetLayout->GetDescriptorSetLayout()};

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
            int frameIndex = renderer.GetFrameIndex();
            FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera, globalDescriptorsSets[frameIndex]};

            GlobalUbo ubo{};
            ubo.projectionView = camera.GetProjectionMatrix() * camera.GetViewMatrix();
            uboBuffers[frameIndex]->WriteToBuffer(&ubo);
            uboBuffers[frameIndex]->Flush();

            renderer.BeginSwapChainRenderPass(commandBuffer);
            renderSystem.RenderObjects(frameInfo, pmeObjects);
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
        cube.transform.translation = {.0f, .1f, 0.0f};
        cube.transform.scale = {.5f, .5f, .5f};
        // cube.transform.translation = {.0f, .0f, .5f};
        pmeObjects.push_back(std::move(cube));
    }
    catch (std::runtime_error ex)
    {
        Logger::Log(LogLevel::Error, ex.what());
    }
}

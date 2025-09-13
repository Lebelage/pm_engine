#pragma once
#include <exception>
#include <iostream>
#include <array>

#include "Core/include/pme_camera.h"
#include "Core/include/pme_device.h"
#include "Core/include/pme_pipeline.h"
#include "Core/include/pme_objects.h"
#include "Core/include/pme_frame_info.h"

#include <glm/gtc/constants.hpp>

namespace pme
{
    struct SimplePushConstantData
    {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    class RenderSystem
    {
    public:
        RenderSystem(PmeDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;

    public:
        void RenderObjects(FrameInfo &frameInfo, std::vector<PmeObject> &gameObjects);

    private:
        void CreatePipeLineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeLine(VkRenderPass renderPass);

    private:
        PmeDevice& device;
        VkPipelineLayout pipelineLayout;
        std::unique_ptr<PmePipeline> pPipeline;
    };

}
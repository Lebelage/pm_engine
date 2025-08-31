#pragma once
#include <exception>
#include <iostream>
#include <array>

#include "Core/include/pme_camera.h"
#include "Core/include/pme_device.h"
#include "Core/include/pme_pipeline.h"
#include "Core/include/pme_objects.h"

#include <glm/gtc/constants.hpp>

namespace pme
{
    struct SimplePushConstantData
    {
        glm::mat4 transform{1.f};
        alignas(16) glm::vec3 color;
    };

    class RenderSystem
    {
    public:
        RenderSystem(PmeDevice &device, VkRenderPass renderPass);
        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;

    public:
        void RenderObjects(VkCommandBuffer commandBuffer, std::vector<PmeObject>& pmeObjects, const PmeCamera& camera);

    private:
        void CreatePipeLineLayout();
        void CreatePipeLine(VkRenderPass renderPass);

    private:
        PmeDevice& device;
        VkPipelineLayout pipelineLayout;
        std::unique_ptr<PmePipeline> pPipeline;
    };

}
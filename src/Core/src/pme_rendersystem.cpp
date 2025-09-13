#include "Core/include/pme_rendersystem.h"

pme::RenderSystem::RenderSystem(PmeDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device{device}
{
    try
    {
        CreatePipeLineLayout(globalSetLayout);
        CreatePipeLine(renderPass);
    }
    catch (std::runtime_error ex)
    {
    }
}

pme::RenderSystem::~RenderSystem()
{
    vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
}

void pme::RenderSystem::CreatePipeLineLayout(VkDescriptorSetLayout globalSetLayout)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    Logger::Log(LogLevel::Success, "Pipeline layout was created");
}

void pme::RenderSystem::CreatePipeLine(VkRenderPass renderPass)
{
    PipelineConfigInfo configInfo{};
    PmePipeline::SetDefaultPipelineConfigInfo(configInfo);
    configInfo.renderPass = renderPass;
    configInfo.pipelineLayout = pipelineLayout;
    pPipeline = std::make_unique<PmePipeline>(device, "shaders/sshader.vert.spv", "shaders/sshader.frag.spv", configInfo);
}

void pme::RenderSystem::RenderObjects(FrameInfo &frameInfo, std::vector<PmeObject> &gameObjects)
{
    pPipeline->Bind(frameInfo.commandBuffer);
    
    vkCmdBindDescriptorSets(
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &frameInfo.globalDescriptorSet,
        0,
        nullptr);

    for (auto &obj : gameObjects)
    {
        SimplePushConstantData push{};
        push.modelMatrix = obj.transform.mat4();
        push.normalMatrix = obj.transform.normalMatrix();

        vkCmdPushConstants(
            frameInfo.commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);

        obj.model->Bind(frameInfo.commandBuffer);
        obj.model->Draw(frameInfo.commandBuffer);
    }
}

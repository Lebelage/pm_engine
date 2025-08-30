#include "Core/include/pme_rendersystem.h"

pme::RenderSystem::RenderSystem(PmeDevice& device, VkRenderPass renderPass) : device{device}
{
    try
    {
        CreatePipeLineLayout();
        CreatePipeLine(renderPass);
    }
    catch (std::runtime_error ex)
    {
    }
}

pme::RenderSystem::~RenderSystem()
{
    vkDestroyPipelineLayout(device.GetDevice(),pipelineLayout,nullptr);
}

void pme::RenderSystem::CreatePipeLineLayout()
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
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

void pme::RenderSystem::RenderObjects(VkCommandBuffer commandBuffer, std::vector<PmeObject>& pmeObjects)
{
    pPipeline->Bind(commandBuffer);

    for (auto &obj : pmeObjects)
    {
        obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
        obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.02f, glm::two_pi<float>());

        SimplePushConstantData push{};
        push.color = obj.color;
        push.transform = obj.transform.mat4();

        vkCmdPushConstants(
            commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);

        obj.model->Bind(commandBuffer);
        obj.model->Draw(commandBuffer);
    }
}

#include "Core/include/pme_app.h"
#include "pme_app.h"

pme::App::App() : window{WIDTH, HEIGHT, "PhysicsModelingEngine"}, device{window}, swapChain{device, window.GetWindowExtent()}
{
    try
    {
        CreatePipeLineLayout();
        CreatePipeLine();
        CreateCommandBuffer();
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
    while (!window.ShouldClose())
    {
        glfwPollEvents();
        DoFrame();
    }
}

void pme::App::DoFrame()
{
    uint32_t imageIndex;
    auto result = swapChain.AcquireNextImage(&imageIndex);

    if(result != VK_SUCCESS)
        throw std::runtime_error("Failed to acqure swap chain image");

    result = swapChain.SubmitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    
    if(result != VK_SUCCESS)
        throw std::runtime_error("Failed to present swap chain image");
}

void pme::App::CreatePipeLineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    Logger::Log(LogLevel::Success, "Pipeline layout was created");
}

void pme::App::CreatePipeLine()
{
    PipelineConfigInfo configInfo{};
    PmePipeline::SetDefaultPipelineConfigInfo(configInfo);
    configInfo.renderPass = swapChain.GetRenderPass();
    configInfo.pipelineLayout = pipelineLayout;
    pPipeline = std::make_unique<PmePipeline>(device, "shaders/sshader.vert.spv", "shaders/sshader.frag.spv", configInfo);
}

void pme::App::CreateCommandBuffer()
{
    commandBuffers.resize(swapChain.GetImageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.GetCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command buffers");

    Logger::Log(LogLevel::Success, "Command buffers were created");

    for (size_t i = 0; i < commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("Failed to begin recording command buffer");

        

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain.GetRenderPass();
        renderPassInfo.framebuffer = swapChain.GetFrameBuffer(i);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.GetSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f,0.1f,0.1f,1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        pPipeline->Bind(commandBuffers[i]);

        vkCmdDraw(commandBuffers[i],3,1,0,0);
        vkCmdEndRenderPass(commandBuffers[i]);

        if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to record command buffer");
    }

    Logger::Log(LogLevel::Success, "Command buffers were created");
}

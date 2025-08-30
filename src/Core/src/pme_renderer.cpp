#include "pme_renderer.h"

pme::PmeRenderer::PmeRenderer(Window &window, PmeDevice &device) : window{window}, device{device}
{
    RecreateSwapChain();
    CreateCommandBuffers();
}

pme::PmeRenderer::~PmeRenderer()
{
    FreeCommandBuffers();
}

VkCommandBuffer pme::PmeRenderer::BeginFrame()
{
    if (isFrameStarted)
        throw std::runtime_error("Failed to call BeginFrame function because frame is already started");

    auto result = pSwapChain->AcquireNextImage(&currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to acqure swap chain image");

    isFrameStarted = true;

    auto commandBuffer = TryGetCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed to begin recording command buffer");

    return commandBuffer;
}

void pme::PmeRenderer::EndFrame()
{
    if (!isFrameStarted)
        throw std::runtime_error("Failed to call EndFrame function because frame is not started");

    auto commandBuffer = TryGetCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to record command buffer");

    auto result = pSwapChain->SubmitCommandBuffers(&commandBuffer, &currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized())
    {
        window.ResetWindowResizedFlag();
        RecreateSwapChain();
    }
    else if(result != VK_SUCCESS)
        throw std::runtime_error("Failed to present swap chain image");

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % PmeSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void pme::PmeRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    if(isFrameStarted){}
    if(commandBuffer == TryGetCurrentCommandBuffer())
    {}

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pSwapChain->GetRenderPass();
    renderPassInfo.framebuffer = pSwapChain->GetFrameBuffer(currentImageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = pSwapChain->GetSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(pSwapChain->GetSwapChainExtent().width);
    viewport.height = static_cast<float>(pSwapChain->GetSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{{0, 0}, pSwapChain->GetSwapChainExtent()};

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void pme::PmeRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    if(isFrameStarted){}
    if(commandBuffer == TryGetCurrentCommandBuffer())
    {}
    vkCmdEndRenderPass(commandBuffer);
}

void pme::PmeRenderer::CreateCommandBuffers()
{
    commandBuffers.resize(PmeSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.GetCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command buffers");

    Logger::Log(LogLevel::Success, "Command buffers were created");
}
void pme::PmeRenderer::RecreateSwapChain()
{
    auto extent = window.GetWindowExtent();
    while (extent.width == 0 || extent.height == 0)
    {
        extent = window.GetWindowExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.GetDevice());
    if (pSwapChain == nullptr)
    {
        pSwapChain = std::make_unique<PmeSwapChain>(device, extent);
    }
    else
    {
        std::shared_ptr<PmeSwapChain> pOldSwapChain = std::move(pSwapChain);
        pSwapChain = std::make_unique<PmeSwapChain>(device, extent, pOldSwapChain);

        if(!pOldSwapChain->CompareSwapFormats(*pSwapChain.get())){}
            //throw std::runtime_error("Swap chain image or depth formats has changed");
    }
}

void pme::PmeRenderer::FreeCommandBuffers()
{
    vkFreeCommandBuffers(
        device.GetDevice(),
        device.GetCommandPool(),
        static_cast<uint32_t>(commandBuffers.size()),
        commandBuffers.data());
    commandBuffers.clear();
}

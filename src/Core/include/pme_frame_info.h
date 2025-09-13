#pragma once
#include "Core/include/pme_camera.h"
#include <vulkan/vulkan.h>
namespace pme
{
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        PmeCamera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}
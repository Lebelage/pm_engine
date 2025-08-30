#pragma once
#include "Core/include/pme_device.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace pme
{
    struct Vertex
    {
        glm::vec2 positions;

        static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription();
    };

    class PmeModel
    {
    public:
        PmeModel(PmeDevice &device, const std::vector<Vertex> &verticies);
        ~PmeModel();

        PmeModel(const PmeModel &) = delete;
        PmeModel operator=(const PmeModel &) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex> &verticies);

    private:
        PmeDevice &device;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
    };
}
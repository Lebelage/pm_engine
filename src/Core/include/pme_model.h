#pragma once
#include "Core/include/pme_device.h"
#include "Core/include/pme_buffer.h"

#include "Utils/include/pme_hash.h"
// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace pme
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
        glm::vec3 uv;
        static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription();

        bool operator==(const Vertex &other) const
        {
            return position == other.position &&
                   color == other.color &&
                   normal == other.normal &&
                   uv == other.uv;
        }
    };

    struct Builder
    {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        void LoadModel(const std::string &filePath);
    };
    class PmeModel
    {
    public:
        PmeModel(PmeDevice &device, const pme::Builder &builder);
        ~PmeModel();

        PmeModel(const PmeModel &) = delete;
        PmeModel operator=(const PmeModel &) = delete;

        static std::unique_ptr<PmeModel> CreateModelFromFile(PmeDevice &device, const std::string &filePath);

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex> &verticies);
        void CreateIndexBuffers(const std::vector<uint32_t> &indices);

    private:
        PmeDevice &device;
        
        std::unique_ptr<PmeBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<PmeBuffer> indexBuffer;
        uint32_t indexCount;
    };
}


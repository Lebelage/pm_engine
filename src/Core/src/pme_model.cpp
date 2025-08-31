#include "Core/include/pme_model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "Core/include/tiny_obj_loader.h"


namespace std
{
    template <>
    struct hash<pme::Vertex>
    {
        size_t operator()(pme::Vertex const &vertex) const
        {
            size_t seed = 0;
            pme::Hash::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

pme::PmeModel::PmeModel(PmeDevice &device, const Builder &builder) : device{device}
{
    CreateVertexBuffers(builder.vertices);
    CreateIndexBuffers(builder.indices);
}

pme::PmeModel::~PmeModel()
{
    vkDestroyBuffer(device.GetDevice(), vertexBuffer, nullptr);
    vkFreeMemory(device.GetDevice(), vertexBufferMemory, nullptr);

    if (hasIndexBuffer)
    {
        vkDestroyBuffer(device.GetDevice(), indexBuffer, nullptr);
        vkFreeMemory(device.GetDevice(), indexBufferMemory, nullptr);
    }
}

void pme::PmeModel::CreateVertexBuffers(const std::vector<Vertex> &verticies)
{
    try
    {
        vertexCount = static_cast<uint32_t>(verticies.size());
        // if(vertexCount < 3)
        VkDeviceSize bufferSize = sizeof(verticies[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void *data;

        vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, verticies.data(), static_cast<uint32_t>(bufferSize));
        vkUnmapMemory(device.GetDevice(), stagingBufferMemory);

        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory);

        device.CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device.GetDevice(), stagingBufferMemory, nullptr);
    }

    catch (std::runtime_error ex)
    {
    }
}
void pme::PmeModel::CreateIndexBuffers(const std::vector<uint32_t> &indices)
{
    try
    {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer)
            return;

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void *data;

        vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<uint32_t>(bufferSize));
        vkUnmapMemory(device.GetDevice(), stagingBufferMemory);

        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer,
            indexBufferMemory);

        device.CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device.GetDevice(), stagingBufferMemory, nullptr);
    }
    catch (std::runtime_error ex)
    {
    }
}

std::unique_ptr<pme::PmeModel> pme::PmeModel::CreateModelFromFile(pme::PmeDevice &device, const std::string &filePath)
{
    Builder builder{};
    builder.LoadModel(filePath);

    return std::make_unique<PmeModel>(device, builder);
}

void pme::PmeModel::Draw(VkCommandBuffer commandBuffer)
{
    if (hasIndexBuffer)
    {
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        return;
    }
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

void pme::PmeModel::Bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (hasIndexBuffer)
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

std::vector<VkVertexInputBindingDescription> pme::Vertex::GetBindingDescription()
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> pme::Vertex::GetAttributeDescription()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

    attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
    attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
    attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
    attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

    return attributeDescriptions;
}

void pme::Builder::LoadModel(const std::string &filePath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str()))
        throw std::runtime_error(warn + err);

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto &shape : shapes)
    {
        for (const auto &index : shape.mesh.indices)
        {
            Vertex vertex{};

            if (index.vertex_index >= 0)
            {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };

                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };
            }

            if (index.normal_index >= 0)
            {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

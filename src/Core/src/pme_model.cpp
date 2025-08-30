#include "Core/include/pme_model.h"

pme::PmeModel::PmeModel(PmeDevice &device, const std::vector<Vertex> &verticies) : device{device}
{
    CreateVertexBuffers(verticies);
}

pme::PmeModel::~PmeModel()
{
    vkDestroyBuffer(device.GetDevice(), vertexBuffer, nullptr);
    vkFreeMemory(device.GetDevice(), vertexBufferMemory, nullptr);
}

void pme::PmeModel::CreateVertexBuffers(const std::vector<Vertex> &verticies)
{
    try
    {
        vertexCount = static_cast<uint32_t>(verticies.size());
        // if(vertexCount < 3)
        VkDeviceSize bufferSize = sizeof(verticies[0]) * vertexCount;
    
        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer,
            vertexBufferMemory);
    
        void *data;
    
        vkMapMemory(device.GetDevice(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, verticies.data(), static_cast<uint32_t>(bufferSize));
        vkUnmapMemory(device.GetDevice(), vertexBufferMemory);
    }
    catch (std::runtime_error ex)
    {
    }
}

void pme::PmeModel::Draw(VkCommandBuffer commandBuffer)
{
    vkCmdDraw(commandBuffer, vertexCount, 1, 0,0);
}

void pme::PmeModel::Bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0 , 1, buffers, offsets);
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
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, positions);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);


    return attributeDescriptions;
}

#include "Core/include/pme_descriptors.h"

#include <cassert>

namespace pme
{
#pragma region Descriptor Set Layout Builder
    PmeDescriptorSetLayout::Builder &PmeDescriptorSetLayout::Builder::AddBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count)
    {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<PmeDescriptorSetLayout> PmeDescriptorSetLayout::Builder::Build() const
    {
        return std::make_unique<PmeDescriptorSetLayout>(device, bindings);
    }
#pragma endregion

#pragma region Descriptor Set Layout
    PmeDescriptorSetLayout::PmeDescriptorSetLayout(
        PmeDevice &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : device{device}, bindings{bindings}
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
                device.GetDevice(),
                &descriptorSetLayoutInfo,
                nullptr,
                &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    PmeDescriptorSetLayout::~PmeDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(device.GetDevice(), descriptorSetLayout, nullptr);
    }
#pragma endregion

#pragma region Descriptor Pool Builder
    PmeDescriptorPool::Builder &PmeDescriptorPool::Builder::AddPoolSize(
        VkDescriptorType descriptorType, uint32_t count)
    {
        poolSizes.push_back({descriptorType, count});
        return *this;
    }

    PmeDescriptorPool::Builder &PmeDescriptorPool::Builder::SetPoolFlags(
        VkDescriptorPoolCreateFlags flags)
    {
        poolFlags = flags;
        return *this;
    }
    PmeDescriptorPool::Builder &PmeDescriptorPool::Builder::SetMaxSets(uint32_t count)
    {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<PmeDescriptorPool> PmeDescriptorPool::Builder::Build() const
    {
        return std::make_unique<PmeDescriptorPool>(device, maxSets, poolFlags, poolSizes);
    }
#pragma endregion

#pragma region descriptor pool

    PmeDescriptorPool::PmeDescriptorPool(
        PmeDevice &PmeDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes)
        : device{device}
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(PmeDevice.GetDevice(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    PmeDescriptorPool::~PmeDescriptorPool()
    {
        vkDestroyDescriptorPool(device.GetDevice(), descriptorPool, nullptr);
    }

    bool PmeDescriptorPool::AllocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        if (vkAllocateDescriptorSets(device.GetDevice(), &allocInfo, &descriptor) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }

    void PmeDescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet> &descriptors) const
    {
        vkFreeDescriptorSets(
            device.GetDevice(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void PmeDescriptorPool::ResetPool()
    {
        vkResetDescriptorPool(device.GetDevice(), descriptorPool, 0);
    }

#pragma endregion

#pragma region descriptor writer
    PmeDescriptorWriter::PmeDescriptorWriter(PmeDescriptorSetLayout &setLayout, PmeDescriptorPool &pool)
        : setLayout{setLayout}, pool{pool} {}

    PmeDescriptorWriter &PmeDescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo *bufferInfo)
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    PmeDescriptorWriter &PmeDescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo *imageInfo)
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    bool PmeDescriptorWriter::Build(VkDescriptorSet &set)
    {
        bool success = pool.AllocateDescriptor(setLayout.GetDescriptorSetLayout(), set);
        if (!success)
        {
            return false;
        }
        Overwrite(set);
        return true;
    }

    void PmeDescriptorWriter::Overwrite(VkDescriptorSet &set)
    {
        for (auto &write : writes)
        {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool.device.GetDevice(), writes.size(), writes.data(), 0, nullptr);
    }
#pragma endregion
}
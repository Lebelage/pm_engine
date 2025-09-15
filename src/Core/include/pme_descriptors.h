#pragma once
/// Core
#include "Core/include/pme_device.h"
/// std
#include <memory>
#include <unordered_map>
#include <vector>
namespace pme
{
     class PmeDescriptorSetLayout
    {
    public:
        class Builder
        {
        public:
            Builder(PmeDevice& device) : device{device} {}

            Builder &AddBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<PmeDescriptorSetLayout> Build() const;

        private:
            PmeDevice& device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        PmeDescriptorSetLayout(PmeDevice &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~PmeDescriptorSetLayout();
        PmeDescriptorSetLayout(const PmeDescriptorSetLayout &) = delete;
        PmeDescriptorSetLayout &operator=(const PmeDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        PmeDevice& device;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class PmeDescriptorWriter;
    };

    class PmeDescriptorPool
    {
    public:
        class Builder
        {
        public:
            Builder(PmeDevice& device) : device{device} {}

            Builder &AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &SetPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &SetMaxSets(uint32_t count);
            std::unique_ptr<PmeDescriptorPool> Build() const;

        private:
            PmeDevice& device;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        PmeDescriptorPool(
            PmeDevice &device,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~PmeDescriptorPool();
        PmeDescriptorPool(const PmeDescriptorPool &) = delete;
        PmeDescriptorPool &operator=(const PmeDescriptorPool &) = delete;

        bool AllocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void FreeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void ResetPool();

    private:
        PmeDevice &device;
        VkDescriptorPool descriptorPool;

        friend class PmeDescriptorWriter;
    };

    class PmeDescriptorWriter
    {
    public:
        PmeDescriptorWriter(PmeDescriptorSetLayout &setLayout, PmeDescriptorPool &pool);

        PmeDescriptorWriter &WriteBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        PmeDescriptorWriter &WriteImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool Build(VkDescriptorSet &set);
        void Overwrite(VkDescriptorSet &set);

    private:
        PmeDescriptorSetLayout &setLayout;
        PmeDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}
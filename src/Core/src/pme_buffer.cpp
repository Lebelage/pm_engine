#include "Core/include/pme_buffer.h"
#include <assert.h>

namespace pme
{
#pragma region Contructor
    PmeBuffer::PmeBuffer(
        PmeDevice &device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment)
        : device{device},
          instanceSize{instanceSize},
          instanceCount{instanceCount},
          usageFlags{usageFlags},
          memoryPropertyFlags{memoryPropertyFlags}
    {
        alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        device.CreateBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
    }
#pragma endregion

#pragma region Destructor
    PmeBuffer::~PmeBuffer()
    {
        Unmap();
        vkDestroyBuffer(device.GetDevice(), buffer, nullptr);
        vkFreeMemory(device.GetDevice(), memory, nullptr);
    }
#pragma endregion

#pragma region Methods

    VkDeviceSize PmeBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
    {
        if (minOffsetAlignment > 0)
        {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }
    VkResult PmeBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
    {
        assert(buffer && memory && "Called map on buffer before create");
        if (size == VK_WHOLE_SIZE)
        {
            return vkMapMemory(device.GetDevice(), memory, 0, bufferSize, 0, &mapped);
        }
        return vkMapMemory(device.GetDevice(), memory, offset, size, 0, &mapped);
    }
    void PmeBuffer::Unmap()
    {
        if (mapped)
        {
            vkUnmapMemory(device.GetDevice(), memory);
            mapped = nullptr;
        }
    }

    void PmeBuffer::WriteToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset)
    {
        assert(mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE)
        {
            memcpy(mapped, data, bufferSize);
        }
        else
        {
            char *memOffset = (char *)mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    VkResult PmeBuffer::Flush(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(device.GetDevice(), 1, &mappedRange);
    }

    VkDescriptorBufferInfo PmeBuffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
    {
        return VkDescriptorBufferInfo{
            buffer,
            offset,
            size,
        };
    }

    VkResult PmeBuffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(device.GetDevice(), 1, &mappedRange);
    }

    void PmeBuffer::WriteToIndex(void *data, int index)
    {
        WriteToBuffer(data, instanceSize, index * alignmentSize);
    }
    VkResult PmeBuffer::FlushIndex(int index) { return Flush(alignmentSize, index * alignmentSize); }

    VkDescriptorBufferInfo PmeBuffer::DescriptorInfoForIndex(int index)
    {
        return DescriptorInfo(alignmentSize, index * alignmentSize);
    }

    VkResult PmeBuffer::InvalidateIndex(int index)
    {
        return Invalidate(alignmentSize, index * alignmentSize);
    }

#pragma endregion
}
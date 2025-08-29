#pragma once
#include "Core/include/pme_device.h"
#include "Utils/include/pme_filereader.h"
#include "Utils/include/pme_logger.h"
#include <string>
#include <vector>
namespace pme
{
    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    class PmePipeline
    {
    public:
        PmePipeline(PmeDevice &device,
                    const std::string &vertFilepath,
                    const std::string &fragFilepath,
                    const PipelineConfigInfo &configInfo);

        ~PmePipeline();

        PmePipeline(const PmePipeline &) = delete;
        PmePipeline &operator=(const PmePipeline &) = delete;

        static void SetDefaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

    public:
        void Bind(VkCommandBuffer commandBuffer);
        void Release();

    private:
        void CreateGraphicsPipeline(
            const std::string &vertFilepath,
            const std::string &fragFilepath,
            const PipelineConfigInfo &configInfo);

        void CreateShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

    private:
        PmeDevice &device;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };

}
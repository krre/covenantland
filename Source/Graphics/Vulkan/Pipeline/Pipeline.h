#pragma once
#include "../Base/Handle.h"
#include "../Device.h"
#include "../ShaderModule.h"
#include "PipelineLayout.h"
#include "../RenderPass.h"

namespace Vulkan {

enum class PipelineType {
    Compute,
    Graphics
};

class Pipeline : public Handle<VkPipeline> {

public:
    Pipeline(PipelineType type, const Device* device);
    ~Pipeline();
    PipelineType getType() const { return type; }
    VkResult addShaderCode(VkShaderStageFlagBits stage, const char* entryPoint, size_t size, const uint32_t* code);
    void setExtent(VkExtent2D extent) { this->extent = extent; }
    void setPipelineLayout(const PipelineLayout* pipelineLayout) { this->pipelineLayout = pipelineLayout; }
    void setRenderPass(const RenderPass* renderPass) { this->renderPass = renderPass; }
    void setVertexBindingDescriptions(const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions);
    void setVertexAttributeDescriptions(const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions);
    VkResult create() override;

private:
    const Device* device;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    PipelineType type;

    std::map<VkShaderStageFlagBits, std::shared_ptr<Vulkan::ShaderModule>> shaderModules;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
    VkExtent2D extent;
    const PipelineLayout* pipelineLayout;
    const RenderPass* renderPass;
};

} // Vulkan
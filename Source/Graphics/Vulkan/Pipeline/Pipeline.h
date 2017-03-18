#pragma once
#include "../Base/Handle.h"
#include "../Device/Devicer.h"
#include "../ShaderModule.h"
#include "PipelineLayout.h"
#include "PipelineCache.h"

namespace Vulkan {

class Pipeline : public Handle<VkPipeline>, public Devicer {

public:
    Pipeline(const Device* device) : Devicer(device) {}
    ~Pipeline();
    void destroy() override;
    VkResult addShaderCode(VkShaderStageFlagBits stage, size_t size, const uint32_t* code, const char* entryPoint = "main");
    void setPipelineCache(VkPipelineCache pipelineCache);
    VkPipelineBindPoint getBindPoint() const { return pipelineBindPoint; }

protected:
    VkPipelineBindPoint pipelineBindPoint;
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;
    std::map<VkShaderStageFlagBits, std::shared_ptr<Vulkan::ShaderModule>> shaderModules;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
};

} // Vulkan

#pragma once
#include "../Base/Handle.h"
#include "../Device/Devicer.h"

namespace Vulkan {

class PipelineCache : public Handle<VkPipelineCache>, public Devicer {

public:
    PipelineCache(Device* device);
    ~PipelineCache();
    void create() override;
    void destroy() override;

private:
    VkPipelineCacheCreateInfo createInfo = {};
};

} // Vulkan

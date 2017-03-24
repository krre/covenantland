#pragma once
#include "../Base/Handle.h"
#include "Devicer.h"

namespace Vulkan {

class DeviceMemory : public Handle<VkDeviceMemory>, public Devicer {

public:
    DeviceMemory(const Device* device);
    ~DeviceMemory();
    void create() override {}
    void destroy() override;
    VkResult allocate(VkDeviceSize size);
    void setMemoryTypeIndex(uint32_t index);
    VkDeviceSize getSize() const { return allocateInfo.allocationSize; }
    VkResult map(VkDeviceSize count, VkDeviceSize offset, void** data);
    void unmap();

private:
    VkMemoryAllocateInfo allocateInfo = {};
};

} // Vulkan

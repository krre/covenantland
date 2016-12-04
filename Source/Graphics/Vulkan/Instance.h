#pragma once
#include "Base/CreatableObject.h"
#include <vector>

namespace Vulkan {

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class Instance : public CreatableObject<VkInstance> {

public:
    Instance();
    ~Instance();
    bool create() override;

    uint32_t getLayerCount() const { return layers.size(); }
    VkLayerProperties getLayer(int i) const { return layers.at(i); }
    void setEnabledLayers(const std::vector<const char*> enabledLayers);
    void dumpLayers();

    uint32_t getExtensionCount() const { return extensions.size(); }
    VkExtensionProperties getExtension(int i) const { return extensions.at(i); }
    void setEnabledExtensions(const std::vector<const char*> enabledExtensions);
    void dumpExtensions();

private:
    VkApplicationInfo appInfo = {};
    VkInstanceCreateInfo createInfo = {};
    std::vector<VkLayerProperties> layers;
    std::vector<const char*> enabledLayers;
    std::vector<VkExtensionProperties> extensions;
    std::vector<const char*> enabledExtensions;
};

} // Vulkan

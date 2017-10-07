#pragma once
#ifdef __linux__
    #define VK_USE_PLATFORM_XCB_KHR
#elif _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "Base/Handle.h"
#include <vector>

class RenderWindow;

namespace Vulkan {

class Swapchain;

class Surface : public Handle<VkSurfaceKHR> {

public:
    Surface(VkPhysicalDevice physicalDevice, RenderWindow* window);
    ~Surface();
    void create() override;
    void destroy() override;

    const std::vector<VkSurfaceFormatKHR>& getFormats() const { return formats; }
    const std::vector<VkPresentModeKHR>& getPresentModes() const { return presentModes; }
    const VkSurfaceCapabilitiesKHR& getCapabilities() const { return capabilities; }
    VkExtent2D getCurrentExtent() const;
    Swapchain* getSwapchain() const { return swapchain.get(); }

private:
    RenderWindow* window;
    VkPhysicalDevice physicalDevice;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    VkSurfaceCapabilitiesKHR capabilities;
    std::unique_ptr<Swapchain> swapchain;
};

} // Vulkan

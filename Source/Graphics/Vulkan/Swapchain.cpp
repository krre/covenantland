#include "Swapchain.h"

using namespace Vulkan;

int Swapchain::indexCounter = 0;

Swapchain::Swapchain(const Device* device, const Surface* surface) :
    Devicer(device),
    surface(surface) {
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;

    VkBool32 surfaceSupport;
    vkGetPhysicalDeviceSurfaceSupportKHR(device->getPhysicalDevice()->getHandle(), 0, surface->getHandle(), &surfaceSupport);
    if (surfaceSupport) {
        createInfo.surface = surface->getHandle();
        const uint32_t desirableImageCount = 3; // Triple buffering
        createInfo.minImageCount = std::min(surface->getCapabilities().maxImageCount, desirableImageCount);
        createInfo.imageFormat = surface->getFormat(0).format;
        createInfo.imageColorSpace = surface->getFormat(0).colorSpace;
        createInfo.imageExtent = surface->getCapabilities().currentExtent;
        createInfo.preTransform = surface->getCapabilities().currentTransform;
        createInfo.presentMode = surface->getPresentMode(0);
    }
}

Swapchain::~Swapchain() {
    destroy();
}

void Swapchain::create() {
    CHECK_RESULT(vkCreateSwapchainKHR(device->getHandle(), &createInfo, nullptr, &handle), "Failed to create swapchain");

    uint32_t count;
    vkGetSwapchainImagesKHR(device->getHandle(), handle, &count, nullptr);
    images.resize(count);
    vkGetSwapchainImagesKHR(device->getHandle(), handle, &count, images.data());

    index = indexCounter++;
}

void Swapchain::destroy() {
    VULKAN_DESTROY_HANDLE(vkDestroySwapchainKHR(device->getHandle(), handle, nullptr))
}

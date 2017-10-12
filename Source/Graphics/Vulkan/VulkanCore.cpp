#include "VulkanCore.h"
#include "Graphics/Vulkan/Wrapper/Instance.h"
#include "Graphics/Vulkan/Wrapper/Device/PhysicalDevices.h"
#include "Graphics/Vulkan/Wrapper/Device/PhysicalDevice.h"
#include "Graphics/Vulkan/Wrapper/Device/Device.h"
#include "Graphics/Vulkan/Wrapper/Command/CommandPool.h"

VulkanCore::VulkanCore() {
    instance = std::make_unique<Vulkan::Instance>();
    instance->create();

    physicalDevices = std::make_unique<Vulkan::PhysicalDevices>(instance->getHandle());

    // Create graphics logical device
    Vulkan::PhysicalDevice* gpd = physicalDevices->findDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    if (gpd == nullptr) {
        gpd = physicalDevices->findDevice(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
    }

    graphicsDevice = std::make_unique<Vulkan::Device>(gpd);
    graphicsFamily = gpd->findQueueFamily(VK_QUEUE_GRAPHICS_BIT);
    graphicsDevice->addQueueCreateInfo(graphicsFamily, { 1.0 });
    graphicsDevice->create();

    // Create compute logical device
    Vulkan::PhysicalDevice* cpd = physicalDevices->findDevice(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
    if (cpd == nullptr) {
        cpd = physicalDevices->findDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    }

    computeDevice = std::make_unique<Vulkan::Device>(gpd);
    computeFamily = gpd->findQueueFamily(VK_QUEUE_COMPUTE_BIT);
    computeDevice->addQueueCreateInfo(computeFamily, { 1.0 });
    computeDevice->create();

    commandPool = std::make_shared<Vulkan::CommandPool>(graphicsDevice.get(), graphicsFamily);
    commandPool->create();
}

VulkanCore::~VulkanCore() {
    if (graphicsDevice) {
        graphicsDevice->waitIdle();
    }
    if (computeDevice) {
        computeDevice->waitIdle();
    }
}
#include "Queue.h"
#include "Semaphore.h"
#include "Fence.h"
#include "Command/CommandBuffer.h"
#include "Surface/Swapchain.h"

namespace Origin {

namespace Vulkan {

Queue::Queue(Device* device, uint32_t queueFamilyIndex, uint32_t queueIndex) : Devicer(device) {
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    vkGetDeviceQueue(device->getHandle(), queueFamilyIndex, queueIndex, &handle);
}

void Queue::addPresentWaitSemaphore(Semaphore* semaphore) {
    presentWaitSemaphores.push_back(semaphore->getHandle());
}

void Queue::clearPresentWaitSemaphores() {
    presentWaitSemaphores.clear();
}

void Queue::waitIdle() {
    VULKAN_CHECK_RESULT(vkQueueWaitIdle(handle), "Failed to wait idle for queue");
}

void Queue::syncHost(Fence* fence) {
    VULKAN_CHECK_RESULT(vkQueueSubmit(handle, 0, nullptr, fence->getHandle()), "Failed to sync host with queue");
}

void Queue::submit(Fence* fence) {
    VULKAN_CHECK_RESULT(vkQueueSubmit(handle, submitInfos.size(), submitInfos.data(), fence == nullptr ? VK_NULL_HANDLE : fence->getHandle()), "Failed to submit queue");
}

void Queue::addCommandBuffer(CommandBuffer* commandBuffer, Semaphore* signalSemaphore, Semaphore* waitSemaphore, VkPipelineStageFlags waitDstStageMask) {
    commandBuffers.push_back(commandBuffer->getHandle());

    if (signalSemaphore != nullptr) {
        submitSignalSemaphores.push_back(signalSemaphore->getHandle());
    }

    if (waitSemaphore != nullptr) {
        submitWaitSemaphores.push_back(waitSemaphore->getHandle());
        submitWaitDstStageMasks.push_back(waitDstStageMask);
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask = submitWaitDstStageMasks.data();
    submitInfo.waitSemaphoreCount = submitWaitSemaphores.size();
    submitInfo.pWaitSemaphores = submitWaitSemaphores.data();
    submitInfo.signalSemaphoreCount = submitSignalSemaphores.size();
    submitInfo.pSignalSemaphores = submitSignalSemaphores.data();
    submitInfo.commandBufferCount = commandBuffers.size();
    submitInfo.pCommandBuffers = commandBuffers.data();

    submitInfos.push_back(submitInfo);
}

void Queue::clearCommandBuffers() {
    commandBuffers.clear();
    submitSignalSemaphores.clear();
    submitWaitSemaphores.clear();
    submitWaitDstStageMasks.clear();
    submitInfos.clear();
}

void Queue::present(uint32_t* indices) {
    assert(!swapchainHandles.empty());
    presentInfo.waitSemaphoreCount = presentWaitSemaphores.size();
    presentInfo.pWaitSemaphores = presentWaitSemaphores.data();
    presentInfo.swapchainCount = swapchainHandles.size();
    presentInfo.pSwapchains = swapchainHandles.data();
    presentInfo.pImageIndices = indices == nullptr ? imageIndices.data() : indices;
    VULKAN_CHECK_RESULT(vkQueuePresentKHR(handle, &presentInfo), "Failed to present swapchain image");
}

void Queue::addSwapchain(Swapchain* swapchain) {
    swapchainHandles.push_back(swapchain->getHandle());
    swapchains.push_back(swapchain);
    imageIndices.resize(swapchainHandles.size());

    for (int i = 0; i < swapchains.size(); i++) {
        swapchains.at(i)->setImageIndexPtr(&imageIndices.at(i));
    }
}

void Queue::clearSwapchains() {
    swapchainHandles.clear();
    swapchains.clear();
    imageIndices.clear();
}

} // Vulkan

} // Origin

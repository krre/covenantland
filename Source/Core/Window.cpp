#include "Window.h"
#include "Core/Application.h"
#include "Core/SDLWrapper.h"
#include "Core/Settings.h"
#include "Core/Defines.h"
#include "Core/Utils.h"
#include "Core/Screen.h"
#include "Core/Game.h"
#include "Graphics/Render/RenderContext.h"
#include "Graphics/Render/Renderer.h"
#include "Debug/DebugHUD.h"
#include "Gui/Toast.h"
#include "Event/Event.h"
#include "Event/Input.h"
#include "Core/Defines.h"
#include "Graphics/Vulkan/VulkanRenderContext.h"
#include "Graphics/Vulkan/Wrapper/Surface/Surface.h"
#include "Graphics/Vulkan/Wrapper/Instance.h"
#include "Graphics/Vulkan/Wrapper/Surface/Swapchain.h"
#include "Graphics/Vulkan/Wrapper/Queue/PresentQueue.h"
#include "Graphics/Vulkan/Wrapper/Device/PhysicalDevice.h"
#include "Graphics/Vulkan/Wrapper/Device/DeviceMemory.h"
#include "Graphics/Vulkan/Wrapper/RenderPass.h"
#include "Graphics/Vulkan/Wrapper/Framebuffer.h"
#include "Graphics/Vulkan/Wrapper/Semaphore.h"
#include "Graphics/Vulkan/Wrapper/Fence.h"
#include "Graphics/Vulkan/Wrapper/Image/Image.h"
#include "Graphics/Vulkan/Wrapper/Image/ImageView.h"
#include "Graphics/Vulkan/Wrapper/Command/CommandBufferOneTime.h"
#include "Graphics/Vulkan/VulkanRenderer.h"
#include <lodepng/lodepng.h>
#include <SDL_syswm.h>
#include <SDL_keycode.h>
#include <ctime>
#include <experimental/filesystem>
#include <SDL_video.h>
#include <lodepng/lodepng.h>

#if defined(OS_WIN)
    #include "Graphics/Vulkan/Wrapper/Surface/Win32Surface.h"
#elif defined(OS_LINUX)
    #include "Graphics/Vulkan/Wrapper/Surface/XcbSurface.h"
#endif

namespace Origin {

Window::Window() {
    auto settingsWidth = Settings::get()->getStorage()["width"];
    auto settingsHeigth = Settings::get()->getStorage()["height"];

    if (!settingsWidth.is_null()) {
        width = settingsWidth.get<int>();
    }

    if (!settingsHeigth.is_null()) {
        height = settingsHeigth.get<int>();
    }

    auto settingsX = Settings::get()->getStorage()["x"];
    auto settingsY = Settings::get()->getStorage()["y"];

    Size screenSize = SDLWrapper::getScreenSize();
    x = settingsX.is_null() ? (screenSize.width - width) / 2 : settingsX.get<int>();
    y = settingsY.is_null() ? (screenSize.height - height) / 2 : settingsY.get<int>();

    handle = SDL_CreateWindow(APP_NAME, x, y, width, height, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    if (handle == nullptr) {
        throw std::runtime_error(std::string("Window could not be created\n") + SDL_GetError());
    }

    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    SDL_GetWindowWMInfo(handle, &wminfo);

    device = vkCtx->getGraphicsDevice();

    presentQueue = std::make_unique<Vulkan::PresentQueue>(device, vkCtx->getGraphicsFamily());
    presentQueue->create();

    presentFence = std::make_unique<Vulkan::Fence>(device);
    presentFence->setSignaledBit();
    presentFence->create();

    imageAvailableSemaphore = std::make_unique<Vulkan::Semaphore>(device);
    imageAvailableSemaphore->create();

    renderFinishedSemaphore = std::make_unique<Vulkan::Semaphore>(device);
    renderFinishedSemaphore->create();
    presentQueue->addWaitSemaphore(renderFinishedSemaphore.get());

#if defined(OS_WIN)
    surface = std::make_unique<Vulkan::Win32Surface>(context->getInstance(), device->getPhysicalDevice(), GetModuleHandle(nullptr), wminfo.info.win.window);
#elif defined(OS_LINUX)
    surface = std::make_unique<Vulkan::XcbSurface>(vkCtx->getInstance(), device->getPhysicalDevice(), XGetXCBConnection(wminfo.info.x11.display), wminfo.info.x11.window);
#endif

    surface->create();

    VkExtent2D currentExtent = surface->getCurrentExtent();

    renderPass = std::make_unique<Vulkan::RenderPass>(device);
    renderPass->setColorFormat(surface->getFormats().at(0).format);
    renderPass->setExtent(currentExtent);
    renderPass->create();

    swapchain = std::make_unique<Vulkan::Swapchain>(device, surface.get());

    onResize(width, height);

    Event::get()->windowMove.connect(this, &Window::onMove);
    Event::get()->windowResize.connect(this, &Window::onResize);
    Event::get()->keyPressed.connect(this, &Window::onKeyPressed);
}

Window::~Window() {
    Settings::get()->getStorage()["x"] = x;
    Settings::get()->getStorage()["y"] = y;

    Settings::get()->getStorage()["width"] = width;
    Settings::get()->getStorage()["height"] = height;

    SDL_DestroyWindow(handle);
}

void Window::pushScreen(const std::shared_ptr<Screen>& screen) {
    if (!screens.empty()) {
        screens.back()->pause();
    }
    screens.push_back(screen);
    screen->resize(width, height);
    screen->resume();
}

void Window::popScreen() {
    if (screens.size() > 1) {
        screens.back()->pause();
        screens.pop_back();
        screens.back()->resume();
    } else {
        // TODO: Question dialog about exit from game
        PRINT("Exit question dialog")
    }
}

void Window::setScreen(const std::shared_ptr<Screen>& screen) {
    for (const auto& screen : screens) {
        screen->pause();
    }
    screens.clear();
    pushScreen(screen);
}

void Window::show() {
    assert(handle != nullptr);
    SDL_ShowWindow(handle);
}

void Window::update(float dt) {
    screens.back()->update(dt);
}

void Window::render() {
    presentFence->wait();
    presentFence->reset();

    VkResult result = swapchain->acquireNextImage(imageAvailableSemaphore.get());
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        onResize(surface->getCurrentExtent().width, surface->getCurrentExtent().height);
    }

    RenderContext::get()->getRenderer()->render(screens.back().get());

    presentQueue->present();
    vkQueueSubmit(presentQueue->getHandle(), 0, nullptr, presentFence->getHandle());
}

void Window::onMove(int x, int y) {
    this->x = x;
    this->y = y;
}

void Window::onResize(int width, int height) {
    this->width = width;
    this->height = height;

    swapchain->destroy();
    swapchain->create();
    presentQueue->clearSwapchains();
    presentQueue->addSwapchain(swapchain.get());

    imageViews.clear();
    framebuffers.clear();

    for (const auto& image : swapchain->getImages()) {
        std::unique_ptr<Vulkan::ImageView> imageView = std::make_unique<Vulkan::ImageView>(device, image);
        imageView->setFormat(surface->getFormats().at(0).format);
        imageView->create();

        std::unique_ptr<Vulkan::Framebuffer> framebuffer = std::make_unique<Vulkan::Framebuffer>(device);
        framebuffer->addAttachment(imageView.get());
        framebuffer->setRenderPass(renderPass.get());
        framebuffer->setWidth(width);
        framebuffer->setHeight(height);
        framebuffer->create();

        imageViews.push_back(std::move(imageView));
        framebuffers.push_back(std::move(framebuffer));
    }

    renderPass->setExtent({ (uint32_t)width, (uint32_t)height });

    VulkanRenderer* renderer = static_cast<VulkanRenderer*>(vkCtx->getRenderer());
    if (renderer) {
        renderer->updateCommandBuffers();
    }

    for (const auto& screen : screens) {
        screen->resize(width, height);
    }
}

void Window::saveScreenshot() {
    std::string directoryPath = Application::getCurrentDirectory() + Utils::getPathSeparator() + "Screenshot";
    namespace fs = std::experimental::filesystem;
    if (!fs::exists(directoryPath)) {
        fs::create_directory(directoryPath);
    }

    time_t t = std::time(0); // Get time now
    struct tm* now = std::localtime(&t);
    std::string filename =
            std::to_string(now->tm_year + 1900) + "-" +
            Utils::zeroFill(std::to_string(now->tm_mon + 1)) + "-" +
            Utils::zeroFill(std::to_string(now->tm_mday)) + "_" +
            Utils::zeroFill(std::to_string(now->tm_hour)) + "-" +
            Utils::zeroFill(std::to_string(now->tm_min)) + "-" +
            Utils::zeroFill(std::to_string(now->tm_sec)) + ".png";
    std::string filePath = directoryPath + Utils::getPathSeparator() + filename;

    VkImage srcImage = swapchain->getCurrentImage();

    uint32_t width = surface->getCurrentExtent().width;
    uint32_t height = surface->getCurrentExtent().height;

    Vulkan::Image image(device);
    image.setWidth(width);
    image.setHeight(height);
    image.create();
    VkImage dstImage = image.getHandle();

    Vulkan::CommandBufferOneTime commandBuffer(device, vkCtx->getGraphicsCommandPool());
    commandBuffer.setImageLayout(dstImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
    commandBuffer.setImageLayout(srcImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

    bool supportsBlit = device->getPhysicalDevice()->getSupportBlit(swapchain->getImageFormat());
    if (supportsBlit) {
        VkOffset3D blitSize;
        blitSize.x = width;
        blitSize.y = height;
        blitSize.z = 1;

        VkImageBlit imageBlitRegion = {};
        imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlitRegion.srcSubresource.layerCount = 1;
        imageBlitRegion.srcOffsets[1] = blitSize;
        imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlitRegion.dstSubresource.layerCount = 1;
        imageBlitRegion.dstOffsets[1] = blitSize;

        commandBuffer.addBlitRegion(imageBlitRegion);
        commandBuffer.blitImage(srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    } else {
        VkImageCopy imageCopy;
        imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopy.srcSubresource.mipLevel = 0;
        imageCopy.srcSubresource.baseArrayLayer = 0;
        imageCopy.srcSubresource.layerCount = 1;
        imageCopy.srcOffset = {};
        imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopy.dstSubresource.mipLevel = 0;
        imageCopy.dstSubresource.baseArrayLayer = 0;
        imageCopy.dstSubresource.layerCount = 1;
        imageCopy.dstOffset = {};
        imageCopy.extent.width = width;
        imageCopy.extent.height = height;
        imageCopy.extent.depth = 1;

        commandBuffer.addImageCopy(imageCopy);
        commandBuffer.copyImage(srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    }

    commandBuffer.setImageLayout(dstImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT);
    commandBuffer.apply();

    // Get layout of the image (including row pitch)
    VkImageSubresource subResource = {};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    VkSubresourceLayout subResourceLayout;
    vkGetImageSubresourceLayout(device->getHandle(), dstImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    const unsigned char* data;
    image.getMemory()->map((void**)&data, VK_WHOLE_SIZE);
    data += subResourceLayout.offset;

    if (supportsBlit) {
        lodepng::encode(filePath, data, width, height);
    } else {
        std::vector<unsigned char> output;
        output.resize(width * height * 4);
        // Convert from BGR to RGB
        uint32_t offset = 0;
        for (uint32_t y = 0; y < height; y++) {
            unsigned int *row = (unsigned int*)data;
            for (uint32_t x = 0; x < width; x++) {
                output[offset++] = *((char*)row + 2);
                output[offset++] = *((char*)row + 1);
                output[offset++] = *((char*)row);
                output[offset++] = *((char*)row + 3);

                row++;
            }

            data += subResourceLayout.rowPitch;
        }

        lodepng::encode(filePath, output.data(), width, height);
    }

    image.getMemory()->unmap();

    std::string message = "Screenshot saved to " + filename;
//    Toast::get()->showToast(message);
    PRINT(message)
}

void Window::toggleFullScreen() {
    bool isFullscreen = SDL_GetWindowFlags(handle) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(handle, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    SDL_ShowCursor(isFullscreen);
}

void Window::setColor(const Color& color) {
    this->color = color;
}

void Window::onKeyPressed(const SDL_KeyboardEvent& event) {
    switch (event.keysym.sym) {
#ifdef DEBUG_HUD_ENABLE
    case SDLK_F5:
        DebugHUD::get()->trigger();
        break;
#endif
    case SDLK_F10:
        toggleFullScreen();
        break;
    case SDLK_F11:
        saveScreenshot();
        break;
    }
}

uint32_t Window::getImageIndex() const {
    return swapchain->getImageIndex();
}

} // Origin
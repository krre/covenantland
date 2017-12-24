#pragma once
#include "Common.h"
#include <vector>

class Control;
class View;
class RenderWindow;

namespace Vulkan {
    class Device;
    class CommandBuffer;
    class CommandBuffers;
}

class Screen {

public:
    Screen(const Size& size);
    ~Screen();
    virtual void hide();
    virtual void pause();
    virtual void resume();
    virtual void show();

    void setRootControl(Control* control);

    void update(float dt);
    void resize(uint32_t width, uint32_t height);

    void pushView(const std::shared_ptr<View>& view);
    void popView();
    View* getCurrentView() const { return currentView; }

    Vulkan::CommandBuffer* getCommandBuffer(size_t i) const { return commandBuffers.at(i).get(); }

private:
    void updateRenderViews();

    Control* rootControl = nullptr;
    std::vector<std::shared_ptr<View>> views;
    std::vector<View*> renderViews;
    View* currentView = nullptr;
    bool isPaused = true;
    RenderWindow* window = nullptr;
    Size size;

    Vulkan::Device* device;
    std::unique_ptr<Vulkan::CommandBuffers> commandBufferHandlers;
    std::vector<std::unique_ptr<Vulkan::CommandBuffer>> commandBuffers;
};

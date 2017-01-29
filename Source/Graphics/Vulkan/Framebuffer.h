#pragma once
#include "Base/Handle.h"
#include "Device/Devicer.h"
#include "RenderPass.h"
#include "Image/ImageView.h"

namespace Vulkan {

class Framebuffer : public Handle<VkFramebuffer>, public Devicer {

public:
    Framebuffer(const Device* device, const RenderPass* renderPass, ImageView* imageView, VkExtent2D extent);
    ~Framebuffer();
    VkResult create() override;
    void destroy() override;

private:
    ImageView* imageView;
    VkFramebufferCreateInfo createInfo = {};
};

} // Vulkan

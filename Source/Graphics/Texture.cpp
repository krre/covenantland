#include "Texture.h"
#include <lodepng/lodepng.h>
#include "Vulkan/Manager.h"

Texture::Texture(const std::string& path, VkFormat format) :
        image(Vulkan::Manager::get()->getDevice()) {
    uint32_t width;
    uint32_t height;
    unsigned result = lodepng::decode(data, width, height, path);
    if (result) {
        throw std::runtime_error("Failed to decode image " + path);
    }

    image.setWidth(width);
    image.setHeight(height);
    image.setFormat(format);
    image.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT);
    image.setInitialLayout(VK_IMAGE_LAYOUT_PREINITIALIZED);
    image.create();

    image.write(data.data(), data.size());

    imageView = std::make_shared<Vulkan::ImageView>(Vulkan::Manager::get()->getDevice(), image.getHandle());
    imageView->createInfo.format = image.getFormat();
    imageView->create();
}

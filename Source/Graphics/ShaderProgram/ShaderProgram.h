#pragma once
#include "../../Core/Object.h"
#include "../../Resource/ShaderResource.h"
#include "../Vulkan/Device/Device.h"
#include "../Vulkan/Descriptor/DescriptorPool.h"
#include "../Vulkan/Descriptor/DescriptorSetLayout.h"
#include "../Vulkan/Descriptor/DescriptorSets.h"
#include "../Vulkan/Buffer.h"
#include "../Vulkan/Image/Image.h"
#include <map>

class ShaderProgram : public Object {

public:

    enum class Type {
        VERTEX,
        FRAGMENT,
        COMPUTE
    };

    struct LinkInfo {
        std::string name;
        uint32_t size;
        Vulkan::Buffer* buffer;
        Vulkan::Image* image;
    };

    ShaderProgram(const Vulkan::Device* device);
    ~ShaderProgram();
    void addShader(const std::string& path);
    void createDescriptors();
    void linkBuffer(std::string name, void* uniform, uint32_t size);
    void write(void* uniform, VkDeviceSize offset = 0, VkDeviceSize size = 0, void* data = nullptr);
    void read(void* uniform, VkDeviceSize offset = 0, VkDeviceSize size = 0, void* data = nullptr);

protected:
    std::map<Type, ShaderResource*> shaderResources;
    Vulkan::DescriptorSetLayout descriptorSetLayout;
    Vulkan::DescriptorSets descriptorSets;
    std::map<void*, LinkInfo> uniformLinks;

private:
    const Vulkan::Device* device;
    Vulkan::DescriptorPool descriptorPool;
    std::vector<std::shared_ptr<Vulkan::Buffer>> buffers;
};
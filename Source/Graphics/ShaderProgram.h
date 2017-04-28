#pragma once
#include "Core/Object.h"
#include <vulkan/vulkan.h>
#include <map>
#include <vector>

namespace Vulkan {
    class GraphicsPipeline;
    class PipelineLayout;
    class DescriptorSetLayout;
    class Buffer;
    class DescriptorSets;
    class DescriptorPool;
}

class ShaderResource;

class ShaderProgram : public Object {

public:

    struct BufferInfo {
        uint32_t size;
        void* uniform;
        std::shared_ptr<Vulkan::Buffer> buffer;
    };

    ShaderProgram();
    ~ShaderProgram();
    void addShader(const std::string& path);
    Vulkan::GraphicsPipeline* getGraphicsPipeline() { return graphicsPipeline.get(); }
    const Vulkan::DescriptorSetLayout* getDescriptorSetLayout() const { return descriptorSetLayout.get(); }
    const Vulkan::DescriptorSets* getDescriptorSets() const { return descriptorSets.get(); }
    const Vulkan::PipelineLayout* getPipelineLayout() const { return pipelineLayout.get(); }
    Vulkan::Buffer* getUniformBuffer(const std::string& name) const { return bufferInfos.at(name).buffer.get(); }
    void createPipeline();
    int createVertexInputBindingDescription(uint32_t stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
    void bindUniform(const std::string& name, uint32_t size, void* uniform = nullptr);
    void linkImage(const std::string& name, VkDescriptorImageInfo descriptorImageInfo);
    void bindInput(const std::string& name, uint32_t binding, uint32_t offset = 0);
    void writeUniform(const std::string& name, VkDeviceSize offset = 0, VkDeviceSize size = 0, void* data = nullptr);
    void readUniform(const std::string& name, VkDeviceSize offset = 0, VkDeviceSize size = 0, void* data = nullptr);

private:
    std::unique_ptr<Vulkan::GraphicsPipeline> graphicsPipeline;
    std::unique_ptr<Vulkan::PipelineLayout> pipelineLayout;
    std::unique_ptr<Vulkan::DescriptorPool> descriptorPool;
    std::vector<ShaderResource*> shaderResources;
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    std::unique_ptr<Vulkan::DescriptorSetLayout> descriptorSetLayout;
    std::unique_ptr<Vulkan::DescriptorSets> descriptorSets;
    std::map<std::string, BufferInfo> bufferInfos;
    std::map<std::string, VkVertexInputAttributeDescription> inputInfos;
    std::map<std::string, VkDescriptorImageInfo> imageInfos;
    int vertexBindingCount = 0;
};

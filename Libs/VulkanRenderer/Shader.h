#pragma once
#include <vector>
#include <string>
#include <vulkan/vulkan.h>
#include <spirv_cross/spirv_cross.hpp>

namespace Vulkan {

class Shader {

public:
    Shader();
    ~Shader();
    void load(const std::string& filePath);
    const std::vector<uint32_t>& getCode() const { return code; }

private:
    void parse();
    VkShaderStageFlagBits getStage(spv::ExecutionModel model);

    std::vector<uint32_t> code;
    VkShaderStageFlagBits stage;
};

} // Vulkan

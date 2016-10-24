#include "ResourceManager.h"
#include "../Core/App.h"

ResourceManager::ResourceManager() {
    dataPath = App::getCurrentPath() + "/Data";
    shaderPath = dataPath + "/Shader";
    fontPath = dataPath + "/Font";
    octreePath = dataPath + "/Octree";
    loadAll();
}

void ResourceManager::loadAll() {
    // ShaderGroups
    auto baseShaderGroup = std::make_shared<ShaderGroup>();
    baseShaderGroup->loadShader(GL_VERTEX_SHADER, shaderPath + "/Base.vert");
    baseShaderGroup->loadShader(GL_FRAGMENT_SHADER, shaderPath + "/Base.frag");
    baseShaderGroup->link();
    resources["BaseShaderGroup"] = baseShaderGroup;

    auto fontShaderGroup = std::make_shared<ShaderGroup>();
    fontShaderGroup->loadShader(GL_VERTEX_SHADER, shaderPath + "/Font.vert");
    fontShaderGroup->loadShader(GL_FRAGMENT_SHADER, shaderPath + "/Font.frag");
    fontShaderGroup->link();
    resources["FontShaderGroup"] = fontShaderGroup;

    auto lightShaderGroup = std::make_shared<ShaderGroup>();
    lightShaderGroup->loadShader(GL_VERTEX_SHADER, shaderPath + "/Light.vert");
    lightShaderGroup->loadShader(GL_FRAGMENT_SHADER, shaderPath + "/Light.frag");
    lightShaderGroup->link();
    resources["LightShaderGroup"] = lightShaderGroup;

    auto voxelShaderGroup = std::make_shared<ShaderGroup>();
    voxelShaderGroup->loadShader(GL_VERTEX_SHADER, shaderPath + "/Voxel.vert");
    voxelShaderGroup->loadShader(GL_FRAGMENT_SHADER, shaderPath + "/Voxel.frag");
    voxelShaderGroup->link();
    resources["VoxelShaderGroup"] = voxelShaderGroup;

    auto surfaceShaderGroup = std::make_shared<ShaderGroup>();
    surfaceShaderGroup->loadShader(GL_VERTEX_SHADER, shaderPath + "/Surface.vert");
    surfaceShaderGroup->loadShader(GL_FRAGMENT_SHADER, shaderPath + "/Surface.frag");
    surfaceShaderGroup->link();
    resources["SurfaceShaderGroup"] = surfaceShaderGroup;

    auto raycastShaderGroup = std::make_shared<ShaderGroup>();
    raycastShaderGroup->loadShader(GL_COMPUTE_SHADER, shaderPath + "/Raycast.comp");
    raycastShaderGroup->link();
    resources["RaycastShaderGroup"] = raycastShaderGroup;

    // Fonts
    auto iconsolataFont = std::make_shared<Font>();
    iconsolataFont->load(fontPath + "/inconsolatalgc.ttf");
    resources["IconsolataFont"] = iconsolataFont;

    // Octrees
    auto groundOctree = std::make_shared<Octree>();
    groundOctree->load(octreePath + "/Ground.json");
    resources["GroundOctree"] = groundOctree;

    auto treeOctree = std::make_shared<Octree>();
    treeOctree->load(octreePath + "/Tree.json");
    resources["TreeOctree"] = treeOctree;

    auto chamomileOctree = std::make_shared<Octree>();
    chamomileOctree->load(octreePath + "/Chamomile.json");
    resources["ChamomileOctree"] = chamomileOctree;
}

Font* ResourceManager::getFont(const std::string& name) {
    return static_cast<Font*>(resources[name].get());
}

ShaderGroup* ResourceManager::getShaderGroup(const std::string& name) {
    return static_cast<ShaderGroup*>(resources[name].get());
}

Octree *ResourceManager::getOctree(const std::string& name) {
    return static_cast<Octree*>(resources[name].get());
}

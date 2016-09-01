#include "RenderSurface.h"
#include "../Resource/ResourceManager.h"
#include "Octree.h"
#include "../Core/App.h"
#include "../ECS/Components/Components.h"
#include "../ECS/Systems/CameraSystem.h"
#include "../ECS/Engine.h"
#include "../Core/Common.h"
#include "../Core/Utils.h"
#include <glm/gtx/matrix_decompose.hpp>

RenderSurface::RenderSurface() :
    texture(GL_TEXTURE_RECTANGLE),
    VBO(GL_ARRAY_BUFFER) {

    voxelShaderGroup = ResourceManager::getInstance()->getShaderGroup("VoxelShaderGroup");
    program = voxelShaderGroup->getProgram();
    voxelShaderGroup->use();

    AABB aabb;
    aabb.min = glm::vec3(-1.0, -1.0, -1.0);
    aabb.max = glm::vec3(1.0, 1.0, 1.0);

    glUniform3f(glGetUniformLocation(program, "aabbMin"), aabb.min.x, aabb.min.y, aabb.min.z);
    glUniform3f(glGetUniformLocation(program, "aabbMax"), aabb.max.x, aabb.max.y, aabb.max.z);
//    glProgramUniform3fv(glGetUniformLocation(voxelShaderGroup->getProgram(), "aabb.min"), 1, glm::value_ptr(aabb.min));

    GLfloat vertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,

        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
    };

    VAO.bind();

    VBO.bind();
    VBO.setData(vertices, sizeof(vertices));

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    VAO.unbind();
}

void RenderSurface::draw(float dt) {
    Octree* octree;
    TransformComponent* octreeTransform;
    MaterialComponent* octreeMaterial;
    glm::vec3 octreeColor;

    TransformComponent* lightTransform;
    glm::vec3 lightColor = glm::vec3(0.0);
    glm::vec3 lightPos = glm::vec3(0.0);

    // TODO: Replace by family
    for (auto entity : Engine::getInstance()->getEntities()) {
        OctreeComponent* octreeComp = static_cast<OctreeComponent*>(entity->components[ComponentType::Octree].get());
        if (octreeComp) {
            octree = octreeComp->octree.get();
            octreeTransform = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
            octreeMaterial = static_cast<MaterialComponent*>(entity->components[ComponentType::Material].get());
            octreeColor = octreeMaterial->color;
        }

        LightComponent* lightComp = static_cast<LightComponent*>(entity->components[ComponentType::Light].get());
        if (lightComp) {
            lightTransform = static_cast<TransformComponent*>(entity->components[ComponentType::Transform].get());
            lightColor = lightComp->color;
            lightPos = glm::vec3(lightTransform->objectToWorld[3]);
        }
    }

    int width = App::getInstance()->getWidth();
    int height = App::getInstance()->getHeight();

    Entity* currentCamera = App::getInstance()->getViewport()->getCurrentCamera().get();
    CameraComponent* cameraComp = static_cast<CameraComponent*>(currentCamera->components[ComponentType::Camera].get());
    TransformComponent* cameraTransform = static_cast<TransformComponent*>(currentCamera->components[ComponentType::Transform].get());

    glm::mat4 cameraToObject = octreeTransform->worldToObject * cameraTransform->objectToWorld;

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(cameraToObject, scale, rotation, translation, skew, perspective);

    Ray ray;
    ray.origin = translation;

    glm::vec3 up = cameraComp->up * rotation;
    glm::vec3 look = cameraComp->look * rotation;
    glm::vec3 right = cameraComp->right * rotation;

    // Ray calculation is based on Johns Hopkins presentation:
    // http://www.cs.jhu.edu/~cohen/RendTech99/Lectures/Ray_Casting.bw.pdf
    glm::vec3 h0 = look - up * glm::tan(cameraComp->fov); // min height vector
    glm::vec3 h1 = look + up * glm::tan(cameraComp->fov); // max height vector
    glm::vec3 stepH = (h1 - h0) / height;
    h0 += stepH / 2;

    glm::vec3 w0 = look - right * glm::tan(cameraComp->fov) * width / height; // min width vector
    glm::vec3 w1 = look + right * glm::tan(cameraComp->fov) * width / height; // max width vector
    glm::vec3 stepW = (w1 - w0) / width;
    w0 += stepW / 2;

    glm::vec4 bgColor = App::getInstance()->getViewport()->getBackgroundColor();

    float ambientStrength = 0.1f;

    voxelShaderGroup->use();

    glUniform3f(glGetUniformLocation(program, "backgroundColor"), bgColor.r, bgColor.g, bgColor.b);
    glUniform3f(glGetUniformLocation(program, "octreeColor"), octreeColor.r, octreeColor.g, octreeColor.b);
    glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.r, lightColor.g, lightColor.b);
    glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniformMatrix4fv(glGetUniformLocation(program, "cameraToWorld"), 1, GL_FALSE, glm::value_ptr(cameraTransform->objectToWorld));
    glUniformMatrix4fv(glGetUniformLocation(program, "octreeToWorld"), 1, GL_FALSE, glm::value_ptr(octreeTransform->objectToWorld));
    glUniform3f(glGetUniformLocation(program, "cameraPos"), translation.x, translation.y, translation.z);

    glUniform3f(glGetUniformLocation(program, "w0"), w0.x, w0.y, w0.z);
    glUniform3f(glGetUniformLocation(program, "h0"), h0.x, h0.y, h0.z);
    glUniform3f(glGetUniformLocation(program, "stepW"), stepW.x, stepW.y, stepW.z);
    glUniform3f(glGetUniformLocation(program, "stepH"), stepH.x, stepH.y, stepH.z);

    glUniform1f(glGetUniformLocation(program, "ambientStrength"), ambientStrength);

    VAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    VAO.unbind();
}


#include "Renderer.h"
#include "../ECS/Components/Components.h"
#include "../Core/Game.h"
#include <SDL_opengl.h>
#include <assert.h>

Renderer::Renderer() {

}

void Renderer::render(float dt) {
    assert(entities);

    bool wireframe = Game::getInstance()->getWireframe();
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    for (auto entity : *entities) {
        RenderComponent* renderComp = static_cast<RenderComponent*>(entity->getComponent(Component::Type::Render));
        renderComp->drawable->draw(dt);
    }

    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::setEntities(std::vector<std::shared_ptr<Entity>>* entities) {
    this->entities = entities;
}

#include "Engine.h"
#include "../ECS/Systems/Systems.h"
#include <algorithm>

Engine::Engine() {
    initSystems();
}

void Engine::removeSystem(System::Type type) {
    delete systems.at(type);
    systems.erase(type);
}

System* Engine::getSystem(System::Type type) {
    return systems.at(type);
}

void Engine::clearSystems() {
    for (auto it: systems) {
        delete it.second;
    }
    systems.clear();
}

void Engine::addEntity(std::shared_ptr<Entity> entity) {
    entities.push_back(entity);
}

void Engine::removeEntity(std::shared_ptr<Entity> entity) {
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

std::shared_ptr<Entity> Engine::getEntity(EntityId id) {
    for (auto it: entities) {
        if (it->getId() == id) {
            return it;
        }
    }

    return nullptr;
}

void Engine::clearEntities() {
    entities.clear();
}

void Engine::process(float dt) {
    for (auto it: systems) {
        it.second->process(dt);
    }
}

void Engine::initSystems() {
    // Order important!
    // This is also order of processing
    addSystem<InputSystem>();
    addSystem<TransformSystem>();
    addSystem<NodeSystem>();
    addSystem<RenderSystem>();
}

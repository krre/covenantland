#include "Entity.h"

namespace Origin {

EntityId Entity::counter = 0;

Entity::Entity(Entity* parent) : Object(parent) {
    id = counter++;
}

Entity::Entity(EntityId id, Entity* parent) : Object(parent) {
    this->id = id;
}

Entity::~Entity() {

}

void Entity::setIdCounter(EntityId id) {
    counter = id;
}

void Entity::addComponent(const std::shared_ptr<Component>& component) {
    components[component->getType()] = component;
}

} // Origin

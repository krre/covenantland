#pragma once
#include "../Core/Object.h"
#include "Component.h"
#include <map>

typedef uint64_t EntityId;

class Entity : Object {

public:
    Entity(EntityId id = 0);
    virtual ~Entity();

    template <typename T> T* addComponent() {
        T* component = new T;
        components[component->getType()] = component;
        return component;
    }

    void removeComponent(ComponentType type);
    Component *getComponent(ComponentType type);
    void clearComponents();

    EntityId getId() { return id; }
    map<ComponentType, Component*> getComponents() { return components; }

private:
    map<ComponentType, Component*> components;
    EntityId id;
};

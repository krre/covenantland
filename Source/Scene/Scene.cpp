#include "Scene.h"
#include "../Event/Event.h"
#include "../Graphics/Vulkan/Manager.h"

Scene::Scene() {
    device = Vulkan::Manager::get()->getDevice();
    Event::get()->keyPressed.connect<Scene, &Scene::onKeyPressed>(this);
    Event::get()->windowResize.connect<Scene, &Scene::onWindowResize>(this);
}

Scene::~Scene() {
    Event::get()->keyPressed.disconnect<Scene, &Scene::onKeyPressed>(this);
}

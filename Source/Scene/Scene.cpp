#include "Scene.h"
#include "../Event/Event.h"
#include "../Graphics/Vulkan/Manager.h"

Scene::Scene() {
    device = Vulkan::Manager::get()->getDevice();
    Event::get()->windowResize.connect<Scene, &Scene::onWindowResize>(this);
}

void Scene::pause() {
    Event::get()->keyPressed.disconnect<Scene, &Scene::onKeyPressed>(this);
}

void Scene::resume() {
    Event::get()->keyPressed.connect<Scene, &Scene::onKeyPressed>(this);
}

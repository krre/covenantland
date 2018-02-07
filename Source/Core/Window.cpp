#include "Window.h"
#include "Core/Game.h"
#include "Core/SDLWrapper.h"
#include "Core/Settings.h"
#include "Core/Defines.h"
#include "Screen/Screen.h"
#include "Core/Game.h"
#include "Debug/DebugHUD.h"
#include "UI/Toast.h"
#include "Event/Event.h"
#include "Event/Input.h"
#include "Core/Defines.h"
#include "UI/Overlay.h"
#include "Debug/DebugHUD.h"
#include "Graphics/Render/RenderEngine.h"
#include <lodepng/lodepng.h>
#include <SDL_syswm.h>
#include <SDL_keycode.h>
#include <ctime>
#include <experimental/filesystem>
#include <SDL_video.h>

namespace Origin {

Window::Window() {
    auto settingsWidth = Game::getSettings()->getStorage()["width"];
    auto settingsHeigth = Game::getSettings()->getStorage()["height"];

    if (!settingsWidth.is_null()) {
        width = settingsWidth.get<int>();
    }

    if (!settingsHeigth.is_null()) {
        height = settingsHeigth.get<int>();
    }

    auto settingsX = Game::getSettings()->getStorage()["x"];
    auto settingsY = Game::getSettings()->getStorage()["y"];

    Size screenSize = SDL::getScreenSize();
    x = settingsX.is_null() ? (screenSize.width - width) / 2 : settingsX.get<int>();
    y = settingsY.is_null() ? (screenSize.height - height) / 2 : settingsY.get<int>();

    handle = SDL_CreateWindow(APP_NAME, x, y, width, height, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    if (handle == nullptr) {
        throw std::runtime_error(std::string("Window could not be created\n") + SDL_GetError());
    }

    Game::getEvent()->windowMove.connect(this, &Window::onMove);
    Game::getEvent()->windowResize.connect(this, &Window::onResize);
    Game::getEvent()->keyPressed.connect(this, &Window::onKeyPressed);
}

Window::~Window() {
    Game::getSettings()->getStorage()["x"] = x;
    Game::getSettings()->getStorage()["y"] = y;

    Game::getSettings()->getStorage()["width"] = width;
    Game::getSettings()->getStorage()["height"] = height;

    SDL_DestroyWindow(handle);
}

void Window::pushScreen(const std::shared_ptr<Screen>& screen) {
    if (!screens.empty()) {
        screens.back()->pause();
    }
    screens.push_back(screen);
    screen->resize(width, height);
    screen->resume();
}

void Window::popScreen() {
    if (screens.size() > 1) {
        screens.back()->pause();
        screens.pop_back();
        screens.back()->resume();
    } else {
        // TODO: Question dialog about exit from game
        PRINT("Exit question dialog")
    }
}

void Window::setScreen(const std::shared_ptr<Screen>& screen) {
    for (const auto& screen : screens) {
        screen->pause();
    }
    screens.clear();
    pushScreen(screen);
}

Screen* Window::getCurrentScreen() const {
    return screens.size() ? screens.back().get() : nullptr;
}

void Window::show() {
    assert(handle != nullptr);
    SDL_ShowWindow(handle);
}

void Window::update(float dt) {
    screens.back()->update(dt);
//    DebugHUD::get()->update(dt);
}

void Window::render() {
    screens.back()->draw();
//    DebugHUD::get()->draw();
    RenderEngine::get()->render();
}

void Window::onMove(int x, int y) {
    this->x = x;
    this->y = y;
}

void Window::onResize(int width, int height) {
    this->width = width;
    this->height = height;

    for (const auto& screen : screens) {
        screen->resize(width, height);
    }

//    DebugHUD::get()->resize(width, height);

    RenderEngine::get()->resize();
}

void Window::toggleFullScreen() {
    bool isFullscreen = SDL_GetWindowFlags(handle) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(handle, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    SDL_ShowCursor(isFullscreen);
}

void Window::setColor(const Color& color) {
    this->color = color;
}

void Window::onKeyPressed(const SDL_KeyboardEvent& event) {
    switch (event.keysym.sym) {
#ifdef DEBUG_HUD_ENABLE
    case SDLK_F5:
        Game::getOverlay()->toggleDebugHUD();
        break;
#endif
    case SDLK_F10:
        toggleFullScreen();
        break;
    case SDLK_F11:
        static_cast<RenderEngine*>(RenderEngine::get())->saveScreenshot();
        break;
    }
}

} // Origin

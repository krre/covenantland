#include "Event.h"
#include "Input.h"
#include "Base/Game.h"
#include "Core/Common.h"

namespace Origin {

Event::Event(Object* parent) : Object(parent) {

}

void Event::handleEvents() {
    Game::getInput()->mouseReset();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Game::quit();
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        windowResize.fire(event.window.data1, event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_MOVED:
                        windowMove.fire(event.window.data1, event.window.data2);
                        break;
                    default:
                        break;
                    }
                break;
            case SDL_MOUSEMOTION:
                Game::getInput()->setMousePos(glm::ivec2(event.motion.x, event.motion.y));
                Game::getInput()->setRelMousePos(glm::ivec2(event.motion.xrel, event.motion.yrel));
                mouseMove.fire(event.motion.x, event.motion.y);
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouseButtonAction.fire(event.button);
                break;
            case SDL_MOUSEBUTTONUP:
                mouseButtonAction.fire(event.button);
                break;
            case SDL_MOUSEWHEEL:
                break;
            case SDL_KEYDOWN:
                Game::getInput()->addKey(event.key.keysym.sym);
                Game::getInput()->isKeyAccepted = false;
                keyPressed.fire(event.key);
                break;
            case SDL_KEYUP:
                Game::getInput()->removeKey(event.key.keysym.sym);
                Game::getInput()->isKeyAccepted = false;
                keyRelease.fire(event.key);
                break;
            case SDL_TEXTINPUT:
                textInput.fire(event.text);
                break;
            case SDL_TEXTEDITING:
//                PRINT("edit " << event.edit.text);
                break;
            default:
                break;
        }
    }
}

} // Origin

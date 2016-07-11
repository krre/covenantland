#pragma once
#include "../Core/Singleton.h"
#include <unordered_set>
#include <SDL_keycode.h>
#include <glm/glm.hpp>

class Input : public Singleton<Input> {

public:
    Input();
    void addKey(SDL_Keycode key);
    void removeKey(SDL_Keycode key);
    bool isKeyPressed(SDL_Keycode key);
    void setMousePos(const glm::ivec2& pos);
    void setRelMousePos(const glm::ivec2& relPos);
    const glm::ivec2& getMousePos() const { return mousePos; }
    const glm::ivec2& getRelMousePos() const { return relMousePos; }
    void mouseReset();
private:
    std::unordered_set<SDL_Keycode> keys;
    glm::ivec2 mousePos;
    glm::ivec2 relMousePos;
};

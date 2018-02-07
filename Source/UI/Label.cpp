#include "Label.h"
#include "Resource/ResourceManager.h"
#include "Font.h"
#include "Core/Game.h"
#include "Screen/Screen.h"
#include "UI/UIRenderer.h"
#include "UIBatch.h"

namespace Origin {

Label::Label(Control* parent) : Control(parent) {
    setFont(Game::getResourceManager()->load<Font>("Fonts/inconsolatalgc.ttf"));
}

Label::Label(const std::string& text, Control* parent) :
        text(text),
        Control(parent) {
    setFont(Game::getResourceManager()->load<Font>("Fonts/inconsolatalgc.ttf"));
}

void Label::setText(const std::string& text) {
    this->text = text;
    updateTextData();
}

void Label::setFont(Font* font) {
    this->font = font;
}

void Label::setColor(const Color& color) {
    this->color = color;
}

void Label::drawImpl() {
    UIBatch batch(Game::getUIRenderer()->getVerticles());
    batch.color = color;
    batch.texture = font->getTexture();
    batch.addText(absolutePosition.x, absolutePosition.y, text, font);
    Game::getUIRenderer()->addBatch(batch);
}

void Label::updateTextData() {

}

} // Origin

#include "Drawable.h"

using namespace CovenantLand;

Drawable::Drawable() {

}

void Drawable::setVisible(bool visible) {
    this->visible = visible;
}

void Drawable::setOpacity(float opacity) {
    this->opacity = opacity;
}

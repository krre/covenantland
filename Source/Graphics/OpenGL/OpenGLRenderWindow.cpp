#include "OpenGLRenderWindow.h"
#include "Core/Defines.h"
#include <GL/glew.h>

OpenGLRenderWindow::OpenGLRenderWindow() {
    handle = SDL_CreateWindow(APP_NAME, x, y, width, height, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (handle == nullptr) {
        throw std::runtime_error(std::string("Window could not be created\n") + SDL_GetError());
    }

    context = SDL_GL_CreateContext(handle);

    if (context == nullptr) {
        throw std::runtime_error(std::string("OpenGL context could not be created\n") + SDL_GetError());
    }

    SDL_GL_MakeCurrent(handle, context);

    if (!SDL_GL_ExtensionSupported("GL_ARB_shader_storage_buffer_object") || !SDL_GL_ExtensionSupported("GL_ARB_compute_shader")) {
        throw std::runtime_error(std::string("OpenGL 4.3 is not supported.\nUpdate your video driver"));
    }
}

void OpenGLRenderWindow::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderWindow::swapBuffers() {
    SDL_GL_SwapWindow(handle);
}

void OpenGLRenderWindow::saveImage(const std::string& filePath) {

}

void OpenGLRenderWindow::resize(int width, int height) {

}

void OpenGLRenderWindow::setColorImpl(const Color& color) {
    glClearColor(color.getRed(), color.getGreen(), color.getBlue(), 1.0);
}

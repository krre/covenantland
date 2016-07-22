#include "Mesh.h"
#include <assert.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

Mesh::Mesh() : VBO(GL_ARRAY_BUFFER) {

}

void Mesh::setup() {
    assert(verticles.size());
    VAO.bind();
    VBO.bind();
    VBO.setData(&verticles[0], verticles.size() * sizeof(verticles[0]));

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    VAO.unbind();
}

void Mesh::draw() {
    VAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, verticles.size() / 6);
    VAO.unbind();
}

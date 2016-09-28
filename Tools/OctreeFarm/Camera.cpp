#include "Camera.h"
#include <QtCore>
#include <iostream>

Camera::Camera(QObject* parent) : QObject(parent) {
    reset();
}

void Camera::setCameraToWorld(const glm::mat4 cameraToWorld) {
    m_cameraToWorld = cameraToWorld;
    m_worldToCamera = glm::inverse(m_cameraToWorld);
}

void Camera::setTarget(const glm::vec3& target) {
    m_target = target;
    distance = glm::distance(m_position, target);
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    setCameraToWorld(glm::translate(m_cameraToWorld, position));
}

void Camera::rotate(float yaw, float pitch) {
    m_yaw = yaw;
    m_pitch = pitch;
    update();
}

void Camera::zoom(float amount) {
    m_position += m_look * amount;
    distance = glm::distance(m_position, m_target);
    distance = std::max(minDistance, std::min(distance, maxDistance));
    update();
}

void Camera::reset() {
    m_position = glm::vec3(0.0, 0.0, 3.0);
    setTarget(glm::vec3(0.0, 0.0, 0.0));
    m_scale = 1.0;
    m_yaw = 0;
    m_pitch = 0;
    m_up = glm::vec3(0.0, 1.0, 0.0);
    m_look = glm::vec3(0.0, 0.0, -1.0);
    m_right = glm::vec3(1.0, 0.0, 0.0);
    update();
}

void Camera::update() {
    glm::mat4 R = glm::yawPitchRoll(glm::radians(m_yaw), glm::radians(m_pitch), 0.0f);
    glm::vec3 T = glm::vec3(0, 0, distance);
    T = glm::vec3(R * glm::vec4(T, 0.0f));
    m_position = m_target + T;
    m_look = glm::normalize(m_target - m_position);
    m_up = glm::vec3(R * glm::vec4(UP, 0.0f));
    m_right = glm::cross(m_look, m_up);
    m_worldToCamera = glm::lookAt(m_position, m_target, m_up);
    m_cameraToWorld = glm::inverse(m_worldToCamera);
}

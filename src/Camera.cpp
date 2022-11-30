#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    m_position = position;
    m_direction = direction;
    m_up = up;
}

glm::vec3 Camera::get_look_at()
{
    glm::vec3 target = { 0, 0, 1 };
    glm::mat4 camera_rotation = glm::mat4(1.0);
    m_direction = camera_rotation * glm::vec4(target, 1);
    return m_position + m_direction;
}

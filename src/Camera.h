#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;

    glm::vec3 get_look_at(void);
};

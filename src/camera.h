#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 direction);

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 get_look_at(void);
};

#endif

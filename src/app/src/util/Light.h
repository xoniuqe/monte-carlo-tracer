#ifndef LIGHT_H
#define LIGHT_H

#include<glm/glm.hpp>

//currently just a point light
class Light {
 public:
    //~Light();
    glm::vec3 position, color;
    float power;
    virtual glm::vec3 getLightDirection(const glm::vec3& origin) const = 0;
};
#endif

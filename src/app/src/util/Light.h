#ifndef LIGHT_H
#define LIGHT_H

#include<glm/glm.hpp>

//currently just a point light
struct Light {
  glm::vec3 position, color;
  float power;
  /*int intersects(const glm::vec3& origin, const glm::vec3& direction) {
    return glm::cross(position - origin, direction) <= 10.f;
  }*/
};
#endif

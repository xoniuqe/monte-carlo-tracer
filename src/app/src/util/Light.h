#ifndef LIGHT_H
#define LIGHT_H

#include<glm/glm.hpp>

//currently just a point light
struct Light {
  glm::vec3 position, color;
  float power;
  //  glm::vec3 getLightDirection(const glm::vec3 origin) = 0;
};
#endif

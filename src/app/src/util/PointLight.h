#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light{
 public:
  PointLight(const glm::vec3& position);
  glm::vec3 getLightDirection(const glm::vec3& origin) const;
};

#endif

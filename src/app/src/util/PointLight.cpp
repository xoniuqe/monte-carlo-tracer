#include "PointLight.h"

PointLight::PointLight(const glm::vec3& position) {
  this->position = position;
}

glm::vec3 PointLight::getLightDirection(const glm::vec3& origin) const {
  return position - origin;
}

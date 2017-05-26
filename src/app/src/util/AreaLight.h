#ifndef AREALIGHT_H
#define AREALIGHT_H

#include "Light.h"
#include <functional>

class AreaLight : public Light {
 public:
  AreaLight(const glm::vec3& center, const float& width, const float& height);
  glm::vec3 getLightDirection(const glm::vec3& origin) const;
 private:
  float height, width;
  std::function<float()> mRandom;
};

#endif

#include "AreaLight.h"
#include <random>
#include <iostream>

AreaLight::AreaLight(const glm::vec3& center, const float& width, const float& height) {
  this->position = center;
  this->width = width;
  this->height = height;

  std::default_random_engine generator;
  std::uniform_real_distribution<float> distribution(-1, 1);//2 * M_PI);
  _random = std::bind(distribution, generator);
}

glm::vec3 AreaLight::getLightDirection(const glm::vec3& origin) const {
  glm::vec3 tmp(position.x + (width * 0.5f * _random()), position.y, position.z + (width * 0.5f * _random()));
  //std::cout << "rP: " << tmp.x << " " << tmp.y << "\n";
  return tmp - origin;
}

#include "ArcBall.h"

#include <iostream>
#include <glm/gtx/quaternion.hpp>

ArcBall::ArcBall() {//const glm::vec3& center) {
  isDragging = false;
  quatCurrent = glm::quat(0,0,0,1);
  quatStart = glm::quat(0,0,0,1);
  //radius = 2.f;//0.5f;
  //this->center = center;
}

void ArcBall::beginDragging() {
  isDragging = true;
  vecStart = vecCurrent;
}

void ArcBall::endDragging() {
  isDragging = false;
  quatStart = quatCurrent;//quatEnd = quatCurrent;
}

void ArcBall::setMouse(const glm::vec3& mousePosition) {
  vecCurrent = mousePosition;
}

void ArcBall::place(const glm::vec3& center, float radius) {
  this->center = center;
  this->radius = radius;
}

glm::vec3 ArcBall::mapToSphere(const glm::vec3& pos) {
  glm::vec3 result = (1.f / radius) * (pos - center);
  float dist = glm::dot(result,result);//glm::length(result * result);
  //dist *= dist;
  if(dist > 1.f) {
    glm::normalize(result);
    result.z = 0.f;
  } else {
    result.z = sqrtf(1.0 - dist);
  }
  return result;
}

glm::quat ArcBall::calcQuaternion(const glm::vec3& from, const glm::vec3& to) {
  glm::vec3 tmp = glm::cross(from,to);
  //std::cout << tmp.x << " " << tmp.y << " " << tmp.z << " " << glm::dot(from, to) << "\n";
  return glm::normalize(glm::quat(glm::dot(from,to), tmp.x, tmp.y, tmp.z));// ,  glm::dot(from, to)));
}

glm::mat4 ArcBall::getRotationMatrix() {
  glm::vec3 from = mapToSphere(vecStart);
  glm::vec3 to = mapToSphere(vecCurrent);
  //std::cout << "from: " << from.x << " " << from.y << " " << from.z << "\n";
  //std::cout << "to: " << to.x << " " << to.y << " " << to.z << "\n";
  if(isDragging) {
    quatCurrent = calcQuaternion(from,to) * quatStart;  
  }
  return glm::transpose(glm::toMat4(quatCurrent));
}

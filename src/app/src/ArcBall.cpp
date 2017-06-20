#include "ArcBall.h"

#include <iostream>
#include <glm/gtx/quaternion.hpp>

ArcBall::ArcBall() {//const glm::vec3& _center) {
  _is_dragging = false;
  _quat_current = glm::quat(0,0,0,1);
  _quat_start = glm::quat(0,0,0,1);
  //radius = 2.f;//0.5f;
  //this->_center = _center;
}

ArcBall::~ArcBall() {}

void ArcBall::beginDragging() {
  _is_dragging = true;
  _vec_start = _vec_current;
}

void ArcBall::endDragging() {
  _is_dragging = false;
  _quat_start = _quat_current;//quatEnd = _quat_current;
}

void ArcBall::setMouse(const glm::vec3& mouse_position) {
  _vec_current = mouse_position;
}

void ArcBall::place(const glm::vec3& center, float radius) {
  _center = center;
  _radius = radius;
}

glm::vec3 ArcBall::mapToSphere(const glm::vec3& pos) {
  glm::vec3 result = (1.f / _radius) * (pos - _center);
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
  glm::vec3 from = mapToSphere(_vec_start);
  glm::vec3 to = mapToSphere(_vec_current);
  //std::cout << "from: " << from.x << " " << from.y << " " << from.z << "\n";
  //std::cout << "to: " << to.x << " " << to.y << " " << to.z << "\n";
  if(_is_dragging) {
    _quat_current = calcQuaternion(from,to) * _quat_start;  
  }
  return glm::transpose(glm::toMat4(_quat_current));
}

#include "Camera.h"

#include<glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& origin, const glm::vec3& center, const float& fov, const float& ratio, int screenWidth, int screenHeight){
  _origin = origin;
  _center = center;
  _direction = glm::normalize(this->_origin - this->_center);
  _fov = fov;
  _ar = ratio;
  _width = screenWidth;
  _height = screenHeight;
  _projection = glm::perspective(fov, ratio, 0.1f, 100.0f);
  _up = glm::vec3(0,1,0);
  _view = glm::lookAt(_origin, _center, _up);
  _rotation = glm::mat4(1.f); 

  calculateScreenToWorld();
}

void Camera::rotate(const glm::mat4& rotation) {
  _current_rotation = rotation * _rotation;
  //mCurOrigin = glm::vec3(rotation * glm::vec4(_origin,1.f));
  //mCurUp = glm::vec3(rotation * glm::vec4(_up,0.f));
  //mView = glm::lookAt(mCurOrigin, _center, mCurUp);
  //_origin = newOrigin;
  //_up = newUp;
  //mOrigion *= rotation;
  //mView = rotation * mView;
}

void Camera::applyRotation() {
  _rotation = _current_rotation;
  //mCurrentRotation = mRotation;
}

glm::mat4 Camera::getRotationMatrix() const {
  return _current_rotation;
}

void Camera::calculateScreenToWorld() {
  glm::mat4 inverseProjection= glm::inverse(_projection);
  glm::mat4 inverseView = glm::inverse(_view);// * mRotation;
  //float invWidth = 1.f / (float) _width; 
  //float invHeight = 1.f / (float) _height; 
  glm::vec4 a(-1,-1,-1,1);
  glm::vec4 b(1,-1,-1,1);
  glm::vec4 c(1,1,-1,1);
  glm::vec4 d(-1,1,-1,1);
  glm::vec4 p0,p1,p2,p3,px,py;
  p0 = inverseProjection * a;
  p1 = inverseProjection * b;
  p2 = inverseProjection * c;
  p3 = inverseProjection * d;
  p0 = p0 / p0.w;
  p1 = p1 / p1.w;
  p2 = p2 / p2.w;
  p3 = p3 / p3.w;
  p0 = inverseView * p0;
  p1 = inverseView * p1;
  p2 = inverseView * p2;
  p3 = inverseView * p3;
  _dx = p1 - p0;
  _dx = p3 - p0;
  //mDeltaX *= invWidth;
  //mDeltaY *= invHeight;
  _p0 = p0;
}

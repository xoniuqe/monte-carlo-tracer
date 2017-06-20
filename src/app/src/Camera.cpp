#include "Camera.h"

#include<glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& origin, const glm::vec3& center, const float& fov, const float& ratio){
  _origin = origin;
  mCenter = center;
  mDirection = glm::normalize(_origin - this->mCenter);
  mFieldOfView = fov;
  mAspectRatio = ratio;
  _projection = glm::perspective(fov, ratio, 0.1f, 100.0f);
  mUp = glm::vec3(0,1,0);
  _view = glm::lookAt(_origin, mCenter, mUp);
  mRotation = glm::mat4(1.f); 

  calculateScreenToWorld();
}

void Camera::rotate(const glm::mat4& rotation) {
  mCurrentRotation = rotation * mRotation;
  //mCurOrigin = glm::vec3(rotation * glm::vec4(mOrigin,1.f));
  //mCurUp = glm::vec3(rotation * glm::vec4(mUp,0.f));
  //mView = glm::lookAt(mCurOrigin, mCenter, mCurUp);
  //mOrigin = newOrigin;
  //mUp = newUp;
  //mOrigion *= rotation;
  //mView = rotation * mView;
}

void Camera::applyRotation() {
  mRotation = mCurrentRotation;
  //mCurrentRotation = mRotation;
}

glm::mat4 Camera::getRotationMatrix() const {
  return mCurrentRotation;
}

void Camera::calculateScreenToWorld() {
  glm::mat4 inverseProjection= glm::inverse(_projection);
  glm::mat4 inverseView = glm::inverse(_view);// * mRotation;
  //float invWidth = 1.f / (float) mWidth; 
  //float invHeight = 1.f / (float) mHeight; 
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
  _dy = p3 - p0;
  //mDeltaX *= invWidth;
  //mDeltaY *= invHeight;
  _p0 = p0;
}

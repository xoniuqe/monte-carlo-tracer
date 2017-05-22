#include "Camera.h"

#include<glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& origin, const glm::vec3& center, const float& fov, const float& ratio, int screenWidth, int screenHeight){
  mOrigin = origin;
  mCenter = center;
  mDirection = glm::normalize(this->mOrigin - this->mCenter);
  mFieldOfView = fov;
  mAspectRatio = ratio;
  mWidth = screenWidth;
  mHeight = screenHeight;
  mProjection = glm::perspective(fov, ratio, 0.1f, 100.0f);
  mView = glm::lookAt(mOrigin, mCenter, glm::vec3(0,1,0));


  calculateScreenToWorld();
}

void Camera::calculateScreenToWorld() {
  glm::mat4 inverseProjection= glm::inverse(mProjection);
  glm::mat4 inverseView = glm::inverse(mView);
  float invWidth = 1.f / (float) mWidth; 
  float invHeight = 1.f / (float) mHeight; 
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
  mDeltaX = p1 - p0;
  mDeltaY = p3 - p0;
  mDeltaX *= invWidth;
  mDeltaY *= invHeight;
  mP0 = p0;
}

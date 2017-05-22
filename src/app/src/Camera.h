#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
 public:
  Camera(const glm::vec3& origin, const glm::vec3& center, const float& fov, const float& ratio, int screenWidth, int screenHeight);
  void calculateScreenToWorld();

  glm::vec3 mOrigin, mDirection, mCenter;
  int mWidth, mHeight;
  float mFieldOfView, mAspectRatio;
  glm::mat4 mProjection, mView;
  glm::vec4 mP0, mDeltaX, mDeltaY;

};
#endif

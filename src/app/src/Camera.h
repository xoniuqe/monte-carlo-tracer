#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
 public:
  Camera(const glm::vec3& origin, const glm::vec3& center, const float& fov, const float& ratio, int screenWidth, int screenHeight);
  void calculateScreenToWorld();
  void rotate(const glm::mat4& rotation);
  void applyRotation();
  glm::mat4 getRotationMatrix() const;
  glm::vec3 mOrigin, mDirection, mCenter, mUp;//, mCurUp, mCurOrigin;
  int mWidth, mHeight;
  float mFieldOfView, mAspectRatio;
  glm::mat4 mProjection, mView, mRotation;
  glm::vec4 mP0, mDeltaX, mDeltaY;
 private:
  glm::mat4 mCurrentRotation;

};
#endif

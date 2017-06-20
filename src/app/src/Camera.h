#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
 public:
  Camera(const glm::vec3& origin, const glm::vec3& center, const float& fov, const float& ratio);
  void calculateScreenToWorld();
  void rotate(const glm::mat4& rotation);
  void applyRotation();
  glm::mat4 getRotationMatrix() const;
  glm::vec3 _origin, mDirection, mCenter, mUp;//, mCurUp, mCurOrigin;
  float mFieldOfView, mAspectRatio;
  glm::mat4 _projection, _view, mRotation;
  glm::vec4 _p0, _dx, _dy;
 private:
  glm::mat4 mCurrentRotation;

};
#endif

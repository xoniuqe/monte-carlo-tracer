#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

struct Camera {

  glm::vec3 mOrigin, mDirection, mCenter;
  int mWidth, mHeight;
  float mFieldOfView, mAspectRatio;
  glm::mat4 mProjection, mView;
};
#endif

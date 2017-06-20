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
  glm::vec3 _origin, _direction, _center, _up;//, mCurUp, mCurOrigin;
  int _width, _height;
  float _fov, _ar;
  glm::mat4 _projection, _view, _rotation;
  glm::vec4 _p0, _dx, _dy;
 private:
  glm::mat4 _current_rotation;

};
#endif

#ifndef ARCBALL_H
#define ARCBALL_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class ArcBall {
 public:
  ArcBall();//const glm::vec3& center);
  ~ArcBall();
  glm::mat4 getRotationMatrix();
  void beginDragging();
  void endDragging();
  void setMouse(const glm::vec3& mouse_position);
  void place(const glm::vec3& center, float radius);
  bool _is_dragging;
 private:
  glm::vec3 _vec_current, _vec_start, _center;
  glm::quat _quat_current, _quat_start;
  float _radius;

  glm::vec3 mapToSphere(const glm::vec3& pos);
  glm::quat calcQuaternion(const glm::vec3& from, const glm::vec3& to);

};

#endif

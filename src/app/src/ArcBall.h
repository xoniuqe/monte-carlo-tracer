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
  void setMouse(const glm::vec3& mousePosition);
  void place(const glm::vec3& center, float radius);
  bool isDragging;
 private:
  glm::vec3 vecCurrent, vecStart, center;
  glm::quat quatCurrent, quatStart;
  float radius;

  glm::vec3 mapToSphere(const glm::vec3& pos);
  glm::quat calcQuaternion(const glm::vec3& from, const glm::vec3& to);

};

#endif

#ifndef MC_PATHTRACER_H
#define MC_PATHTRACER_H

#include <functional>
#include <glm/glm.hpp>
#include "Scene.h"
#include "Camera.h"

class MonteCarloPathtracer {
 public:
    MonteCarloPathtracer(Scene* scene, Camera* camera, int screen_width, int screen_height, int num_samples=8, int max_depth=2, int aa=0);
  ~MonteCarloPathtracer();
  void startPathtracing();
  glm::u8vec3* _image;
 private:
  std::function<float()> _random;
  int _num_samples;
  Scene* _scene;
  Camera* _camera;
  int _screen_width, _screen_height;
  int _max_depth;
  int _aa;
  glm::vec3 traceRay(const glm::vec3& origin, const glm::vec3& direction, const int depth, const glm::vec3& view_direction);//const Vertex& vert, const int depth);//const glm::vec3& normal, const int depth);
  glm::vec3 randomDirection(const glm::vec3& n);
};
#endif

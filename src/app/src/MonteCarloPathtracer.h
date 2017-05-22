#ifndef MC_PATHTRACER_H
#define MC_PATHTRACER_H

#include <functional>
#include <glm/glm.hpp>
#include "Scene.h"
#include "Camera.h"

class MonteCarloPathtracer {
 public:
  MonteCarloPathtracer(Scene* scene, Camera* camera, int numSamples=8);
  ~MonteCarloPathtracer();
  void startPathtracing();
  glm::u8vec3* mImage;
 private:
  std::function<float()> mRandom;
  int mNumSamples;
  Scene* mScene;
  Camera* mCamera;
  int mScreenWidth, mScreenHeight;

  glm::u8vec3 traceRay(const glm::vec3& origin, const glm::vec3& normal, const int depth);
  glm::vec3 randomDirection(const glm::vec3& n);
};
#endif

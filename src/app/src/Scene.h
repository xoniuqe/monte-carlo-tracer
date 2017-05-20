#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/glm.hpp>
#include "util/Mesh.h"
#include "util/Light.h"


class Scene {
 public:
  Scene();
  void render();
  void addMesh(Mesh* mesh);
  void addLight(Light* light);
  int intersection(const glm::vec3& origin, const glm::vec3& dir, float* out_t, glm::vec3* out_n, Mesh* out_mesh) const; 
  std::vector<Light*> mLights;
 private:
  std::vector<Mesh*> mMeshes;
  int mNumMeshes;

};

#endif

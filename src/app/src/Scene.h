#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/glm.hpp>
#include "util/Mesh.h"


class Scene {
 public:
  Scene();
  void render();
  void addMesh(Mesh* mesh);
  int intersection(const glm::vec3& origin, const glm::vec3& dir, float* out_t, Mesh* out_mesh) const;

 private:
  std::vector<Mesh*> mMeshes;
  int mNumMeshes;

};

#endif

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/glm.hpp>
#include "util/Mesh.h"
#include "util/Light.h"
#include "geometry/geometry.h"
#include "geometry/Octree.h"

class Scene {
 public:
  Scene();
  ~Scene();
  void render();
  void addMesh(Mesh* mesh);
  void addLight(Light* light);
  void calculateOctree();
  int intersection(const glm::vec3& origin, const glm::vec3& diretion, float* out_t, Vertex* out, Mesh*& out_mesh) const;
  std::vector<Light*>& lights();
  private:
  Octree* _octree;
  std::vector<Mesh*> _meshes;
  std::vector<Light*> _lights;
};

#endif

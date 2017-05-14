#include "Scene.h"
#include <iostream>

Scene::Scene() {
  
}

void Scene::addMesh(Mesh* mesh) {
  mMeshes.push_back(mesh);
}

void Scene::render() {
  for(auto mesh : mMeshes) {
    mesh->render();
  }
}

int Scene::intersection(const glm::vec3& origin, const glm::vec3& dir, float* out_t, Mesh* out_mesh) const {
  Mesh* collider = 0;
  float t = 0.f;
  int result = 0;
  for(auto mesh : mMeshes) {
    float tmpt = 0.f;
    int intersects = mesh->intersect(origin, dir, &tmpt);
    if(intersects && (result == 0 || tmpt < t)) {
        t = tmpt;
        collider = mesh;
        result = 1;
    }
  }
  *out_t = t;
  out_mesh = collider;
  return result;
}

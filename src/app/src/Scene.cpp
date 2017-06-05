#include "Scene.h"
#include <iostream>
#include <glm/gtc/constants.hpp>
#include "geometry/geometry.h"

Scene::Scene() {
  _octree = new Octree(800);
}

void Scene::addMesh(Mesh* mesh) {
  mMeshes.push_back(mesh);
  for(auto triangle : mesh->triangles()) {
    _octree->insert(triangle);
  }
}

void Scene::addLight(Light* light) {
  mLights.push_back(light);
}

void Scene::render() {
  for(auto mesh : mMeshes) {
    mesh->render();
  }
  _octree->render();
}

void Scene::calculateOctree() {
  _octree->subdivide();
}


int Scene::intersection(const glm::vec3& origin, const glm::vec3& direction, float* out_t, Vertex* out, Mesh* out_mesh) const {
  float best = glm::zero<float>();
  glm::vec3 n, tmp_n;
  float tmp = 0.0;
  Vertex res, tmp_res;
  std::set<Triangle*> possible_intersections;
  int result = 0;

  Intersection::intersectRayOctree(*_octree, origin, direction, &possible_intersections);
  for(auto triangle : possible_intersections) {
    tmp_n = glm::normalize(triangle->a.normal + triangle->b.normal + triangle->c.normal);
    int intersection = Intersection::intersectTriangle(*triangle, origin, direction, &tmp);
    if((!result || tmp < best) && intersection && tmp > 0.00001f && glm::dot(tmp_n, direction) <= glm::zero<float>()) {
      best = tmp;
      n = tmp_n;
      result = 1;
      res = Intersection::barycentricInterpolation(*triangle, origin + direction * tmp);
      res.normal = tmp_n;
    }
  }
  res.position = origin + direction * best;
  *out = res;
  *out_t = best;
  //outmesh noch setzen
  return result;
}

#include "Scene.h"
#include <iostream>
#include <glm/gtc/constants.hpp>
#include "geometry/geometry.h"
#include "geometry/Intersection.h"

Scene::Scene() {
  _octree = new Octree(16);
}

Scene::~Scene() {
    for(auto mesh : _meshes) {
        delete mesh;
    }
    //deleting is bugged because I did not use smart pointers 
    delete _octree;
}

void Scene::addMesh(Mesh* mesh) {
  _meshes.push_back(mesh);
  for(auto triangle : mesh->triangles()) {
      triangle->mesh = mesh;
      _octree->insert(triangle);
  }
}

void Scene::addLight(Light* light) {
  _lights.push_back(light);
}

void Scene::render() {
  for(auto mesh : _meshes) {
    mesh->render();
  }
  _octree->render();
}

void Scene::calculateOctree() {
  _octree->subdivide();
}

std::vector<Light*>& Scene::lights() {
    return _lights;
}


int Scene::intersection(const glm::vec3& origin, const glm::vec3& direction, float* out_t, Vertex* out, Mesh*& out_mesh) const {
  float best = glm::zero<float>();
  glm::vec3 tmp_n;
  float tmp = 0.0;
  Vertex res, tmp_res;
  Mesh* mesh_res = NULL;
  std::set<Triangle*> possible_intersections;
  int result = 0;

  Intersection::intersectRayOctree(*_octree, origin, direction, &possible_intersections);
  for(auto triangle : possible_intersections) {
      //tmp_n = glm::normalize(triangle->a.normal + triangle->b.normal + triangle->c.normal);
    int intersection = Intersection::intersectTriangle(*triangle, origin, direction, &tmp);
    if((!result || tmp < best) && intersection && tmp > 0.00001f ) {//&& glm::dot(tmp_n, direction) <= glm::zero<float>()) {
      tmp_res= Intersection::barycentricInterpolation(*triangle, origin + direction * tmp);
      //res.normal = tmp_n;
      if(glm::dot(tmp_res.normal, direction) <= glm::zero<float>()) {
          best = tmp;
          result = 1;
          mesh_res = triangle->mesh;
          res = tmp_res;
      }
    }
  }
  possible_intersections.clear();
  res.position = origin + direction * best;
  *out = res;
  *out_t = best;
  out_mesh = mesh_res;
  mesh_res = NULL;
  //outmesh noch setzen
  return result;
}

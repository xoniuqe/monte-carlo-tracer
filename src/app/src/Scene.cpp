#include "Scene.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

Scene::Scene() {
  
}

void Scene::addMesh(Mesh* mesh) {
  mMeshes.push_back(mesh);
}

void Scene::addLight(Light* light) {
  mLights.push_back(light);
}

void Scene::render() {
  for(auto mesh : mMeshes) {
    mesh->render();
  }
}


int Scene::intersection(const glm::vec3& origin, const glm::vec3& dir, float* out_t, Vertex* out /*glm::vec3* out_n*/, Mesh* out_mesh) const {
  Mesh* collider = 0;
  float best = glm::zero<float>();
  glm::vec3 n, tmp_n;
  Vertex res, tmp_res;
  int result = 0;
  for(auto mesh : mMeshes) {
    float tmpt = 0.f;
    int intersects = mesh->intersect(origin, dir, &tmpt, &tmp_res);//&tmp_n);
    if(intersects && (result == 0 || tmpt < best) && glm::dot(dir,tmp_res.normal/*tmp_n*/) <= glm::zero<float>() && tmpt > 0.00001f){
        best = tmpt;
        collider = mesh;
        //n = tmp_n;
        res = tmp_res;
        result = 1;
    }
  }
  res.position = origin + dir * best;
  *out = res;
  //*out_n = n; 
  *out_t = best;//(origin + dir * best);
  out_mesh = collider;
  return result;
}

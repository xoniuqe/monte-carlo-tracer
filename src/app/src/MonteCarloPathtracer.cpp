#include "MonteCarloPathtracer.h"
#include <cmath>
#include "util/Mesh.h"
#include "util/Light.h"
#include <iostream>
#include <random>
#include <glm/gtc/constants.hpp>
#include <tbb/tbb.h>
#include <tbb/task.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

#include "util/Material.h"
//#define M_PI 3.14159265358979323846f;

MonteCarloPathtracer::MonteCarloPathtracer(Scene* scene, Camera* camera, int screen_width, int screen_height, int num_samples, int max_depth, int aa) {
  _scene = scene;
  _camera = camera;
  _screen_width = screen_width;
  _screen_height = screen_height;
  _image = new glm::u8vec3[_screen_width * _screen_height];
  _num_samples = num_samples;
  _max_depth = max_depth;
  _aa = aa;
  std::default_random_engine generator;
  std::uniform_real_distribution<float> distribution(0, 1);//2 * M_PI);
  _random = std::bind(distribution, generator);
}
MonteCarloPathtracer::~MonteCarloPathtracer() {
    //delete _scene;
    //delete _camera;
    delete[] _image;
}

void MonteCarloPathtracer::startPathtracing() {
  _camera->calculateScreenToWorld();
  std::cout << "start " << std::endl;
  glm::vec4 dx = _camera->_dx * (1.f / (float) _screen_width);
  glm::vec4 dy = _camera->_dy * (1.f / (float) _screen_height);
  tbb::parallel_for(int(0), _screen_width, [=](int x) { // x = 0; x < width; x++){
          tbb::parallel_for(int(0), _screen_height, [=] (int y) { //for(int y = 0; y < height; y++) {
                  /*                glm::u8vec3 res_color(0,0,0);
                  float ratio = 1.f/5.f;
                  for(auto dx = -0.2f; dx <= 0.2f; dx += 0.4f) {
                      for(auto dy = -0.2f; dy <= 0.f; dy += 0.4f) {
                          glm::vec3 pp = _camera->mP0 + dx * ((float) x + dx + 0.5f) + dy * ((float) y + dy + 0.5f);
                          glm::vec3 direction = glm::vec3(pp) - _camera->mOrigin;//(tx,ty,-1.f);
                          direction = glm::normalize(direction);
                          res_color += (glm::u8vec3) ((glm::clamp(traceRay(_camera->mOrigin, direction, 0), glm::vec3(0,0,0), glm::vec3(1,1,1)) * 255.f) * ratio);
                      }                     
                  }
                  glm::vec3 pp = _camera->mP0 + dx * ((float) x + 0.5f) + dy * ((float) y + 0.5f);
                  glm::vec3 direction = glm::vec3(pp) - _camera->mOrigin;//(tx,ty,-1.f);
                  direction = glm::normalize(direction);
                  res_color += (glm::u8vec3) ((glm::clamp(traceRay(_camera->mOrigin, direction, 0), glm::vec3(0,0,0), glm::vec3(1,1,1)) * 255.f) *ratio);
                  _image[x + (_screen_width * y)] = res_color;//glm::clamp(res_color, glm::u8vec3(0,0,0), glm::u8vec3(255,255,255));
*/
                  glm::vec3 pp = _camera->_p0 + dx * ((float) x + 0.5f) + dy * ((float) y + 0.5f);
                  glm::vec3 direction = glm::vec3(pp) - _camera->_origin;//(tx,ty,-1.f);
                  direction = glm::normalize(direction);
                  _image[x + (_screen_width * y)] = (glm::u8vec3) (glm::clamp(traceRay(_camera->_origin, direction, 0), glm::vec3(0,0,0), glm::vec3(1,1,1)) * 255.f);
              });
      });
}

glm::vec3 MonteCarloPathtracer::traceRay(const glm::vec3& origin, const glm::vec3& direction, /*const Vertex& vert, */const int depth) {//const glm::vec3& normal, const int depth) {
  Mesh* collider = NULL;
  glm::vec3 p,n;
  Vertex vert,v;
  float t = 0.0f;
  glm::vec3 indirect_color(0,0,0);
  glm::vec3 direct_color(0,0,0);
  glm::vec3 reflect_color(0,0,0);
  glm::vec3 refract_color(0,0,0);

  glm::vec3 resulting_color(0,0,0);
  if(_scene->intersection(origin, direction, &t, &vert, collider)) {
      //collider material checken und entsprechend verfahren
      auto material = Material::materials[collider->material()];
      auto reflectivity = material->reflectivity;
      auto refraction = material->refraction_index;
      if(glm::length(material->emitting) > glm::zero<float>()) {
          std::cout << "emitting\n";
// std:return material->emitting;
      }
      if(reflectivity > glm::zero<float>()) {
          reflect_color = traceRay(vert.position, glm::reflect(direction, vert.normal), depth + 1);
      }
      //if(refraction > glm::zero<float>()){
          //        reflect_color = traceRay(vert.position, glm::refract(direction, vert.normal, refraction), depth + 1);
      //}
      for(auto light : _scene->lights()) {
          glm::vec3 lDirection = light->getLightDirection(vert.position);
          float dist = glm::length(lDirection);
          float qdist = dist * dist;
          qdist = 1.f / qdist;
          lDirection = glm::normalize(lDirection);
          int intersects = _scene->intersection(vert.position, lDirection, &t, &v, collider);//&n, collider);
          if(!intersects || (intersects &&  dist - t <= glm::zero<float>())) {
              //dist = (light->power - (dist * dist)) / light->power;
              //dist = 1.f;
              if(dist > glm::zero<float>()){
                  direct_color += light->color * std::max(0.f, glm::dot(lDirection, vert.normal)) * light->power * qdist;// * qdist;// * light->power;
                  if(depth == 0) {
                      //glm::vec3 view = glm::normalize(direction);
                      glm::vec3 R = glm::reflect(-lDirection, vert.normal);
                      float cosAlpha = glm::clamp(glm::dot(-direction,R),0.f,1.f);
                      float specular = pow(cosAlpha, material->shininess);
                      
                      direct_color += light->color * specular * light->power * qdist;// * dist;
                  }
              }
          }else if(dist - t > glm::zero<float>()) {
              //std::cout << "dist: " << dist << " t: " << t << "\n";
          }
      }
      
      if(depth < _max_depth) { // { // 8 ) {}
          //TODO russian roulette
          for(auto i = 0; i < _num_samples; i++) {
              glm::vec3 direction = randomDirection(n);
              indirect_color += traceRay(vert.position, direction, depth +1) * std::max(0.f, glm::dot(direction, vert.normal));
          }
          indirect_color *= 1.f / _num_samples;
      }
      resulting_color = (direct_color + indirect_color) * material->diffuse/*vert.color*/ * (1.f - reflectivity) + reflect_color * reflectivity + refract_color;// * glm::one_over_pi<float>()));// + vert.color;// / 2;//(1.f /(float) _num_samples);
  }
  return resulting_color;//glm::clamp(resulting_color, glm::vec3(0,0,0), glm::vec3(255,255,255));
}

glm::vec3 MonteCarloPathtracer::randomDirection(const glm::vec3& n) {
  float phi = 2.f * M_PI * _random();
  float u = 2.f * _random() -1.f;
  float sinTheta = sqrt(1.f - u * u);
   
  glm::vec3 direction(sinTheta * cos(phi), sinTheta* sin(phi), abs(u));
  if(glm::dot(direction,n) <= glm::zero<float>()){
    direction = - direction;
  }
  return direction;
}

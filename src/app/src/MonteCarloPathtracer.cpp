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

MonteCarloPathtracer::MonteCarloPathtracer(Scene* scene, Camera* camera, int screen_width, int screen_height, int num_samples, int max_depth, int aa) {
    //antialiasing is not implemented
    _scene = scene;
    _camera = camera;
    _screen_width = screen_width;
    _screen_height = screen_height;
    _image = new glm::u8vec3[_screen_width * _screen_height];
    _num_samples = num_samples;
    _max_depth = max_depth;
    _aa = aa;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0, 1);
    _random = std::bind(distribution, generator);
}
MonteCarloPathtracer::~MonteCarloPathtracer() {
    delete[] _image;
}

void MonteCarloPathtracer::startPathtracing() {
    _camera->calculateScreenToWorld();
    std::cout << "start " << std::endl;
    glm::vec4 dx = _camera->_dx * (1.f / (float) _screen_width);
    glm::vec4 dy = _camera->_dy * (1.f / (float) _screen_height);
    //with blockrange
    tbb::parallel_for(tbb::blocked_range2d<int>(0,_screen_width, 0, _screen_height),[=](const tbb::blocked_range2d<int>& r) {
            for(auto x = r.rows().begin(); x != r.rows().end(); ++x) {
                for(auto y = r.cols().begin(); y != r.cols().end(); ++y) {
                    glm::vec3 resulting_color = glm::vec3(0,0,0);
                    //for(auto i = 0; i < _num_samples; ++i) {
                        glm::vec3 pp, direction;
                        pp = _camera->_p0 + dx * ((float) x + 0.5f) + dy * ((float) y + 0.5f);
                        direction = glm::normalize(glm::vec3(pp) - _camera->_origin);//(tx,ty,-1.f)
                        
                        resulting_color = glm::clamp(traceRay(_camera->_origin, direction, 0, direction), glm::vec3(0,0,0), glm::vec3(1,1,1));
                        //}
                    _image[x + (_screen_width * y)] = (glm::u8vec3) (resulting_color * 255.f) ;
                }
            }
        });
    //without blockrange
/*tbb::parallel_for(int(0), _screen_width, [=](int x) { // x = 0; x < width; x++){
            tbb::parallel_for(int(0), _screen_height, [=] (int y) { //for(int y = 0; y < height; y++) {
                    glm::vec3 pp = _camera->_p0 + dx * ((float) x + 0.5f) + dy * ((float) y + 0.5f);
                    glm::vec3 direction = glm::vec3(pp) - _camera->_origin;//(tx,ty,-1.f);
                    direction = glm::normalize(direction);
                    _image[x + (_screen_width * y)] = (glm::u8vec3) (glm::clamp(traceRay(_camera->_origin, direction, 0, direction), glm::vec3(0,0,0), glm::vec3(1,1,1)) * 255.f);
                });
                });*/
    std::cout << "end" << std::endl;
}

glm::vec3 MonteCarloPathtracer::traceRay(const glm::vec3& origin, const glm::vec3& direction, const int depth, const glm::vec3& view_direction) {
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
            return material->emitting;
      }
      if(reflectivity > glm::zero<float>()) {
          reflect_color = traceRay(vert.position, glm::reflect(direction, vert.normal), depth + 1, view_direction);
      }
      //if(refraction > glm::zero<float>()){
      //        reflect_color = traceRay(vert.position, glm::refract(direction, vert.normal, refraction), depth + 1);
      //}
      for(auto light : _scene->lights()) {
          glm::vec3 light_direction = light->getLightDirection(vert.position);
          float dist = glm::length(light_direction);
          float qdist = dist * dist;
          qdist = 1.f / qdist;
          light_direction = glm::normalize(light_direction);
          int intersects = _scene->intersection(vert.position, light_direction, &t, &v, collider);
          if(!intersects || (intersects &&  dist - t <= glm::zero<float>())) {
              if(dist > glm::zero<float>()){
                  direct_color += material->diffuse * light->color * std::max(0.f, glm::dot(light_direction, vert.normal)) * light->power * qdist;
                  glm::vec3 H = glm::normalize(light_direction - view_direction);
                  float cosAlpha = glm::clamp(glm::dot(H,vert.normal),0.f,1.f);
                  float specular = pow(cosAlpha, material->shininess);
                  
                  direct_color += light->color * specular * light->power * qdist;
              }
          }
      }
      
      if(depth < _max_depth && 1.f - reflectivity > glm::zero<float>()) {
          for(auto i = 0; i < _num_samples; i++) {
              glm::vec3 direction = randomDirection(n);
              indirect_color += traceRay(vert.position, direction, depth +1, view_direction) * std::max(0.f, glm::dot(direction, vert.normal));
          }
          indirect_color *= 1.f / _num_samples;
      }
      resulting_color = (direct_color + indirect_color) * (1.f - reflectivity) + reflect_color * reflectivity;// + refract_color ;
  }
  return resulting_color;
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

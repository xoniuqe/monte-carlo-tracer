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
//#define M_PI 3.14159265358979323846f;

MonteCarloPathtracer::MonteCarloPathtracer(Scene* scene, Camera* camera, int numSamples) {
  mScene = scene;
  mCamera = camera;
  mImage = new glm::u8vec3[camera->mWidth * camera->mHeight];
  mNumSamples = numSamples;
  std::default_random_engine generator;
  std::uniform_real_distribution<float> distribution(0, 1);//2 * M_PI);
  mRandom = std::bind(distribution, generator);
}
MonteCarloPathtracer::~MonteCarloPathtracer() {
  delete mScene;
  delete mCamera;
  delete[] mImage;
}

void MonteCarloPathtracer::startPathtracing() {
  std::cout << "start " << std::endl;
  int width = mCamera->mWidth;
  int height = mCamera->mHeight;
  //float dx = ()

  tbb::parallel_for(int(0), width, [=](int x) { // x = 0; x < width; x++){
      tbb::parallel_for(int(0), height, [=] (int y) { //for(int y = 0; y < height; y++) {
      glm::vec3 pp = mCamera->mP0 + mCamera->mDeltaX * ((float) x + 0.5f) + mCamera->mDeltaY * ((float) y + 0.5f);//(p0 + (float) x * px) + (py +(float) y * py);// + 0.5f * px + 0.5f * py;
      glm::vec3 direction = glm::vec3(pp) - mCamera->mOrigin;//(tx,ty,-1.f);
      direction = glm::normalize(direction);
      //if(x < 5 && y < 5)
      if((x == 1 && y == 1) || (x == width-1 && y == height-1))
        std::cout << "direction: " << direction.x << " " << direction.y << " " << direction.z << "\n";
      Mesh *collider;
      glm::vec3 p,n;
      float t;
      int result = mScene->intersection(mCamera->mOrigin, direction, &t, &n, collider);
      if(result == 1 && t > 0.001f) {// && glm::dot(n,mCamera->mDirection) < glm::zero<float>()) {
        p = mCamera->mOrigin + direction * t;
        mImage[x + (width * y)] = traceRay(p,n,0);
        //mImage[x + (width * y)] = glm::u8vec3(255,0,0);
      } else {
        mImage[x + (width * y)] = glm::u8vec3(0,0,0);
      }
        });
    });
}

glm::u8vec3 MonteCarloPathtracer::traceRay(const glm::vec3& origin, const glm::vec3& normal, const int depth) {
  Mesh* collider = NULL;
  glm::vec3 p,n;
  float t = 0.0f;
  glm::u8vec3 indirectColor(0,0,0);
  glm::u8vec3 directColor(0,0,0);
  for(auto light : mScene->mLights) {
    glm::vec3 lDirection = glm::normalize(light->position - origin);
    float dist = glm::length(origin - light->position);
    int intersects = mScene->intersection(origin, lDirection, &t, &n, collider);
    if(!intersects || (intersects &&  dist - t <= glm::zero<float>())) {
      dist = (light->power - (dist * dist)) / light->power;
      if(dist > glm::zero<float>()){
        directColor += light->color * std::max(0.f, glm::dot(lDirection, normal)) * dist;// * light->power;
      }
    }else if(dist - t > glm::zero<float>()) {
      //std::cout << "dist: " << dist << " t: " << t << "\n";
    }
  }
  if(depth < 8) { // { // 8 ) {}
    //TODO russian roulette
    for(auto i = 0; i < mNumSamples; i++) {
      glm::vec3 direction = randomDirection(n);
      int result = mScene->intersection(mCamera->mOrigin, direction, &t, &n, collider);
      if(result){
        p = mCamera->mOrigin + direction * t;
        indirectColor += traceRay(p, n, depth +1);
      }
    }
    indirectColor *= 1.f / mNumSamples;
  }
  return directColor + indirectColor;
}

glm::vec3 MonteCarloPathtracer::randomDirection(const glm::vec3& n) {
  float phi = 2.f * M_PI * mRandom();
  float u = 2.f * mRandom() -1.f;
  float w = sqrt(1.f - u * u);
   
  glm::vec3 direction(w * cos(phi), w* sin(phi), abs(u));
  if(glm::dot(direction,n) <= glm::zero<float>()){
    direction = - direction;
  }
  return direction;
}

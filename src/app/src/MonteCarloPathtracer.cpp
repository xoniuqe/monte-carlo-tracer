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

MonteCarloPathtracer::MonteCarloPathtracer(Scene* scene, Camera* camera, int screenWidth, int screenHeight, int numSamples) {
  mScene = scene;
  mCamera = camera;
  mScreenWidth = screenWidth;
  mScreenHeight = screenHeight;
  mImage = new glm::u8vec3[mScreenWidth * mScreenHeight];
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
  mCamera->calculateScreenToWorld();
  std::cout << "start " << std::endl;
  glm::vec4 deltaX = mCamera->mDeltaX * (1.f / (float) mScreenWidth);
  glm::vec4 deltaY = mCamera->mDeltaY * (1.f / (float) mScreenHeight);
  tbb::parallel_for(int(0), mScreenWidth, [=](int x) { // x = 0; x < width; x++){
      tbb::parallel_for(int(0), mScreenHeight, [=] (int y) { //for(int y = 0; y < height; y++) {
      glm::vec3 pp = mCamera->mP0 + deltaX * ((float) x + 0.5f) + deltaY * ((float) y + 0.5f);
      glm::vec3 direction = glm::vec3(pp) - mCamera->mOrigin;//(tx,ty,-1.f);
      direction = glm::normalize(direction);
      mImage[x + (mScreenWidth * y)] = (glm::u8vec3) (traceRay(mCamera->mOrigin, direction, 0) * 255.f);
        });
    });
}

glm::vec3 MonteCarloPathtracer::traceRay(const glm::vec3& origin, const glm::vec3& direction, /*const Vertex& vert, */const int depth) {//const glm::vec3& normal, const int depth) {
  Mesh* collider = NULL;
  glm::vec3 p,n;
  Vertex vert,v;
  float t = 0.0f;
  glm::vec3 indirectColor(0,0,0);
  glm::vec3 directColor(0,0,0);
  if(mScene->intersection(origin, direction, &t, &vert, collider)) {
    for(auto light : mScene->mLights) {
      glm::vec3 lDirection = light->getLightDirection(vert.position);
      float dist = glm::length(lDirection);
      lDirection = glm::normalize(lDirection);
      int intersects = mScene->intersection(vert.position, lDirection, &t, &v, collider);//&n, collider);
      if(!intersects || (intersects &&  dist - t <= glm::zero<float>())) {
        dist = (light->power - (dist * dist)) / light->power;
        dist = 1.f;
        if(dist > glm::zero<float>()){
          directColor += light->color * std::max(0.f, glm::dot(lDirection, vert.normal)) * dist;// * light->power;
        }
      }else if(dist - t > glm::zero<float>()) {
        //std::cout << "dist: " << dist << " t: " << t << "\n";
      }
    }
 
    if(depth < 1) { // { // 8 ) {}
      //TODO russian roulette
      for(auto i = 0; i < mNumSamples; i++) {
        glm::vec3 direction = randomDirection(n);
        indirectColor += traceRay(vert.position, direction, depth +1) * std::max(0.f, glm::dot(direction, vert.normal));
        /*int intersects = mScene->intersection(mCamera->mOrigin, direction, &t, &v, collider);//&n, collider);
        if(intersects){
          p = mCamera->mOrigin + direction * t;
          indirectColor += (glm::vec3)traceRay(p, v, depth +1) *  std::max(0.f, glm::dot(direction, vert.normal)) *glm::one_over_two_pi<float>();//n, depth +1);
          }*/
      }
      indirectColor *= 1.f / mNumSamples;
    }
    return (directColor + indirectColor) * vert.color;// * glm::one_over_pi<float>()));// + vert.color;// / 2;//(1.f /(float) mNumSamples);
  }
  return directColor;
}

glm::vec3 MonteCarloPathtracer::randomDirection(const glm::vec3& n) {
  float phi = 2.f * M_PI * mRandom();
  float u = 2.f * mRandom() -1.f;
  float sinTheta = sqrt(1.f - u * u);
   
  glm::vec3 direction(sinTheta * cos(phi), sinTheta* sin(phi), abs(u));
  if(glm::dot(direction,n) <= glm::zero<float>()){
    direction = - direction;
  }
  return direction;
}

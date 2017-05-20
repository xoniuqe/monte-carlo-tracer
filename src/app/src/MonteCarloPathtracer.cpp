#include "MonteCarloPathtracer.h"
#include <cmath>
#include "util/Mesh.h"
#include "util/Light.h"
#include <iostream>
#include <random>
#include <glm/gtc/constants.hpp>
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

bool triangle = false;
void MonteCarloPathtracer::startPathtracing() {
  std::cout << "start " << std::endl;
  int width = mCamera->mWidth;
  int height = mCamera->mHeight;
  int pixelCount = 0;
  int hits = 0;
  int totalhits = 0;
  int expectedPixels = width * height;
  float invWidth = 1.f / (float) width;
  float invHeight = 1.f / (float) height;
  float angle = tan(M_PI * 0.5f * mCamera->mFieldOfView / 180.f);
  float distance = (float) ((4.f * invWidth) -1.f) * 0.5 * tan(M_PI * 0.5f * 0.5f * mCamera->mFieldOfView / 180.f);
  glm::vec3 screenCenter = mCamera->mDirection * distance;
  std::cout << "screenDirection " << mCamera->mDirection.x << " " << mCamera->mDirection.y << " " << mCamera->mDirection.z << std::endl;
  glm::mat4 inverseProjection= glm::inverse(mCamera->mProjection);
  glm::mat4 inverseView = glm::inverse(mCamera->mView);

  glm::vec4 a(-1,-1,-1,1);
  glm::vec4 b(1,-1,-1,1);
  glm::vec4 c(1,1,-1,1);
  glm::vec4 d(-1,1,-1,1);
  glm::vec4 p0,p1,p2,p3,px,py;
  p0 = inverseProjection * a;
  p1 = inverseProjection * b;
  p2 = inverseProjection * c;
  p3 = inverseProjection * d;
  p0 = p0 / p0.w;
  p1 = p1 / p1.w;
  p2 = p2 / p2.w;
  p3 = p3 / p3.w;
  p0 = inverseView * p0;
  p1 = inverseView * p1;
  p2 = inverseView * p2;
  p3 = inverseView * p3;
  px = p1 - p0;
  py = p3 - p0;
  px *= invWidth;
  py *= invHeight;
  std::cout << "p0: " << p0.x << " " << p0.y << " " << p0.z << std::endl;
  std::cout << "p1: " << p1.x << " " << p1.y << " " << p1.z << std::endl;
  std::cout << "p2: " << p2.x << " " << p2.y << " " << p2.z << std::endl;
  std::cout << "p3: " << p3.x << " " << p3.y << " " << p3.z << std::endl;  
  std::cout << "px: " << px.x << " " << px.y << " " << px.z << std::endl;
  std::cout << "py: " << py.x << " " << py.y << " " << py.z << std::endl;
  std::cout << "px: " << p0.x + px.x * 800.f << " " << p0.y + px.y * 800.f << " " << p0.z + px.z * 800.f<< std::endl;
  //float dx = ()
  std::cout << "screenCenter: " << screenCenter.x << " " << screenCenter.y << " " << screenCenter.z << std::endl;
  //glm::u8vec3* pixel = mImage;
  for(int x = 0; x < width; x++){
    for(int y = 0; y < height; y++) {
      float tx = (2.f * ((x + 0.5) * invWidth) - 1) * angle * mCamera->mAspectRatio;
      float ty = (1.f - 2.f * ((y + 0.5) * invHeight)) * angle;
      glm::vec3 pp = p0 + px * ((float) x + 0.5f) + py * ((float) y + 0.5f);//(p0 + (float) x * px) + (py +(float) y * py);// + 0.5f * px + 0.5f * py;
      glm::vec3 direction = glm::vec3(pp) - mCamera->mOrigin;//(tx,ty,-1.f);
      if((x == 1 && y == 1) || (x == width-1 && y == height-1))
        std::cout << "direction: " << direction.x << " " << direction.y << " " << direction.z << "\n";
      direction = glm::normalize(direction);
      //direction.x = tx;
      //direction.y = ty;
      //direction.z = -1.f;
      //if(x < 5 && y < 5)
      if((x == 1 && y == 1) || (x == width-1 && y == height-1))
        std::cout << "direction: " << direction.x << " " << direction.y << " " << direction.z << "\n";
      Mesh *collider;
      glm::vec3 p,n;
      float t;
      int result = mScene->intersection(mCamera->mOrigin, direction, &t, &n, collider);
      /*if(collider == testTriangle)
        triangle = true;
      else
      triangle = false;*/
      //std::cout << triangle << "\n";
      if(result == 1){
        totalhits++;
      } 
      if(result == 1) {// && glm::dot(n,mCamera->mDirection) < glm::zero<float>()) {
        p = mCamera->mOrigin + direction * t;
        mImage[x + (width * y)] = traceRay(p,n,0);
        // mImage[x + (width * y)] = glm::u8vec3(255,0,0);
        
        hits++;
      } else {
        
        mImage[x + (width * y)] = glm::u8vec3(0,0,0);
      }
       
      pixelCount++;
      // pixel++;
    }
  }
  std::cout << "hits: " << hits << "/ " << totalhits << " Pixelcount: " << pixelCount << " (" << expectedPixels << ")" << std::endl;
}

glm::u8vec3 MonteCarloPathtracer::traceRay(const glm::vec3& origin, const glm::vec3& normal, const int depth) {
  Mesh* collider = NULL;
  glm::vec3 p,n;
  float t = 0.0f;
  glm::u8vec3 indirectColor(0,0,0);
  glm::u8vec3 directColor(0,0,0);
  for(auto light : mScene->mLights) {
    glm::vec3 lDirection = glm::normalize(light->position - origin);
    if(triangle) {
      std::cout << "p: " << p.x << " " << p.y << " " << p.z << "\n";
      std::cout << "col: " << collider;
    }
    float dist = glm::length(origin - light->position);
    int intersects = mScene->intersection(origin, lDirection, &t, &n, collider);
    if(!intersects || (intersects &&  dist - t <= glm::zero<float>())) {
      if(triangle)
        std::cout << "triangle light!\n";
      //light->power = 6.f;
      dist = (light->power - (dist * dist)) / light->power;
      if(dist > glm::zero<float>()){
      //dist = /(dist * dist);
        directColor += light->color * std::max(0.f, glm::dot(lDirection, normal)) * dist;// * light->power;
      }
    }else if(dist - t > glm::zero<float>()) {
      //std::cout << "dist: " << dist << " t: " << t << "\n";
    }
  }
  //directColor.x = 255; 
  //std::cout << "dc: " << directColor.x << " " << directColor.y << " " << directColor.x << "\n";
  //  int result = mScene->intersection(mCamera->mOrigin, direction, &p, &n, collider);
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

void MonteCarloPathtracer::test() {
  for(int i = 0; i < mCamera->mWidth * mCamera->mHeight; i++){
    glm::uvec3 p = mImage[i];
    std::cout << "p: (" << p.x << ", " << p.y << ", " << p.z << ")\n";
  }
  std::cout << mImage << std::endl;
}

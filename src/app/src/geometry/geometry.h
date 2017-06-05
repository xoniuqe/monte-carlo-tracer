#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <set>
#include <stdexcept>

#include "Octree.h"
#include "OctreeNode.h"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
};

struct Triangle {
  Vertex a,b,c;
  Vertex& operator[] (int id) {
    if(id == 0)
      return a;
    if(id == 1)
      return b;
    if(id == 2)
      return c;
    throw std::out_of_range("Triangle has only 3 elements!");
  }
};

class Intersection {
 private:
  static bool axis_test_x(const glm::vec3& eX, const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& halfsize) {
    float pA = eX.z * vA.y - eX.y * vA.z;
    float pB = eX.z * vB.y - eX.y * vB.z;
    float rad = fabs(eX.z) * halfsize.y + fabs(eX.y) * halfsize.z;
    return (fmin(pA,pB) > rad || fmax(pA,pB) < -rad);
  }

  static bool axis_test_y(const glm::vec3& eX, const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& halfsize) {
    float pA = - eX.z * vA.x + eX.x * vA.z;
    float pB = - eX.z * vB.x + eX.x * vB.z;
    float rad = fabs(eX.z) * halfsize.x + fabs(eX.x) * halfsize.z;
    return (fmin(pA,pB) > rad || fmax(pA,pB) < -rad);
  }

  static bool axis_test_z(const glm::vec3& eX, const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& halfsize) {
    float pA = eX.y * vA.x - eX.x * vA.y;
    float pB = eX.y * vB.x - eX.x * vB.y;
    float rad = fabs(eX.y) * halfsize.x + fabs(eX.x) * halfsize.y;
    return (fmin(pA,pB) > rad || fmax(pA,pB) < -rad);
  }

  static void findMinMax(const float& a, const float& b, const float& c, float* _min, float* _max) {
    *_min = *_max = a;
    if(b < *_min) *_min = b;
    if(c < *_min) *_min = c;
    if(b > *_max) *_max = b;
    if(c > *_max) *_max = c;
  }
 public:
  static int planeBoxOverlap(const glm::vec3& normal, const glm::vec3& vertex, const glm::vec3& maxbox) {
    glm::vec3 _min,_max;
    for(auto dim = 0; dim <= 2; dim++) {
      if(normal[dim] > glm::zero<float>()) {
        _min[dim] = -maxbox[dim] - vertex[dim];
        _max[dim] = maxbox[dim] - vertex[dim];
      } else {
        _min[dim] = maxbox[dim] - vertex[dim];
        _max[dim] = -maxbox[dim] - vertex[dim];
      }
    }
    if(glm::dot(normal,_min) > glm::zero<float>()) return 0;
    if(glm::dot(normal,_max) >= glm::zero<float>()) return 1;
    return 0;
  } 

  static int triangleBoxOverlap(const glm::vec3& center, const glm::vec3& halfsize, const Triangle& triangle) {
    glm::vec3 v0,v1,v2, normal, e0, e1, e2;
    float _min,_max;

    v0 = triangle.a.position - center;
    v1 = triangle.b.position - center;
    v2 = triangle.c.position - center;
  
    e0 = v1 - v0;
    e1 = v2 - v1;
    e2 = v0 - v2;

    //AXISTESTS
    if(axis_test_x(e0,v0,v2,halfsize)) return 0;
    if(axis_test_y(e0,v0,v2,halfsize)) return 0;
    if(axis_test_z(e0,v1,v2,halfsize)) return 0;

    if(axis_test_x(e1,v0,v2,halfsize)) return 0;
    if(axis_test_y(e1,v0,v2,halfsize)) return 0;
    if(axis_test_z(e1,v0,v1,halfsize)) return 0;

    if(axis_test_x(e2,v0,v1,halfsize)) return 0;
    if(axis_test_y(e2,v0,v1,halfsize)) return 0;
    if(axis_test_z(e2,v1,v2,halfsize)) return 0; 

    findMinMax(v0.x,v1.x,v2.x,&_min,&_max);
    if(_min > halfsize.x || _max < -halfsize.x) return 0;

    findMinMax(v0.y,v1.y,v2.y,&_min,&_max);
    if(_min > halfsize.y || _max < -halfsize.y) return 0;

    findMinMax(v0.z,v1.z,v2.z,&_min,&_max);
    if(_min > halfsize.z || _max < -halfsize.z) return 0;

    normal = glm::cross(e0,e1);

    if(!planeBoxOverlap(normal,v0,halfsize)) return 0;

    return 1;
  }

  static int intersectRayOctree(const Octree& octree, const glm::vec3& origin, const glm::vec3& direction, std::set<Triangle*> *result) {
    //std::set<Triangle*> result;// = NULL;
    int index[] = {4, 2, 1};
    unsigned char a = 0;
    glm::vec3 _origin = origin;
    glm::vec3 _direction = direction;
    glm::vec3 halfsize = octree.root().halfsize();
    for(int i = 0; i <= 2; i++){
      if(_direction[i] < glm::zero<float>()) {
        _origin[i] = octree.root().center()[i] * 2.f - origin[i];
        _direction[i] = -direction[i];
        a |= index[i];
      }
    }
    //glm::vec3 t0, t1;
    float t0x = (octree.root().center().x - halfsize.x - _origin.x) / _direction.x;
    float t0y = (octree.root().center().y - halfsize.y - _origin.y) / _direction.y;
    float t0z = (octree.root().center().z - halfsize.z - _origin.z) / _direction.z;
    float t1x = (octree.root().center().x + halfsize.x - _origin.x) / _direction.x;
    float t1y = (octree.root().center().y + halfsize.y - _origin.y) / _direction.z;
    float t1z = (octree.root().center().z + halfsize.z - _origin.z) / _direction.y;
    if(fmax(fmax(t0x,t0y),t0z) < fmin(fmin(t1x,t1y),t1z)) {
      processSubtree(t0x, t0y, t0z, t1x, t1y, t1z, octree.root(), a, result);
      return result->empty();
    }
    return 0;
    //return std::move(result);
  }

  static int firstNode(const float t0x, const float t0y, const float t0z, const float tmx, const float tmy, const float tmz) {
    unsigned char result = 0;
    if(t0x > t0y) {
      if(t0x > t0z) {
        if(tmy < t0x) result |= 2;
        if(tmz < t0x) result |= 1 ;
        return (int) result;
      }
    } else if(t0y > t0z){
      if(tmx < t0y) result |= 4;
      if(tmz < t0y) result |= 1;
      return (int) result;
    }
    if(tmx < t0z) result |= 4;
    if(tmy < t0z) result |= 2;
    return (int) result;
  }

  static int newNode(float tmx, int x, float tmy, int y, float tmz, int z) {
    if(tmx < tmy) {
      if(tmx < tmz) return x;
    } else if (tmy < tmz) return y;
    return z;
  }

  static void processSubtree(const float t0x, const float t0y, const float t0z, const float t1x, const float t1y, const float t1z, OctreeNode& node, unsigned char& a, std::set<Triangle*>* result) {
    int currentNode;
    if(t1x < glm::zero<float>() || t1y < glm::zero<float>() || t1z < glm::zero<float>())
      return;

    if(node.leaf()) {
      for(auto triangle : node.triangles()) {
        result->insert(triangle);
      }
      return;
    }
    float tmx = 0.5f * (t0x + t1x);
    float tmy = 0.5f * (t0y + t1y);
    float tmz = 0.5f * (t0z + t1z);
    currentNode = firstNode(t0x, t0y, t0z, tmx, tmy, tmz);
    do {
      switch(currentNode) {
      case 0:
        processSubtree(t0x, t0y, t0z, tmx, tmy, tmz, *node.children().at(a), a, result);
        currentNode = newNode(tmx, 4, tmy, 2, tmz, 1);
        break;
      case 1:
        processSubtree(t0x, t0y, tmz, tmx, tmy, t1z, *node.children().at(1 ^ a), a, result);
        currentNode = newNode(tmx, 5, tmy, 3, t1z, 8);
        break;
      case 2:
        processSubtree(t0x, tmy, t0z, tmx, t1y, tmz, *node.children().at(2 ^ a), a, result);
        currentNode = newNode(tmx, 6, t1y, 8, tmz, 3);
        break;
      case 3:
        processSubtree(t0x, tmy, tmz, tmx, t1y, t1z, *node.children().at(3 ^ a), a, result);
        currentNode = newNode(tmx, 7, t1y, 8, t1z, 8);
        break;
      case 4:
        processSubtree(tmx, t0y, t0z, t1x, tmy, tmz, *node.children().at(4 ^ a), a, result);
        currentNode = newNode(t1x, 8, tmy, 6, tmz, 5);
        break;
      case 5:
        processSubtree(tmx, t0y, tmz, t1x, tmy, t1z, *node.children().at(5 ^ a), a, result);
        currentNode = newNode(t1x, 8, tmy, 7, t1z, 8);
        break;
      case 6:
        processSubtree(tmx, tmy, t0z, t1x, t1y, tmz, *node.children().at(6 ^ a), a, result);
        currentNode = newNode(t1x, 8, t1y, 8, tmz, 7);
        break;
      case 7:
        processSubtree(tmx, tmy, tmz, t1x, t1y, t1z, *node.children().at(7 ^ a), a, result);
        currentNode = 8;
        break;
      }
    } while(currentNode < 8);
  }

  static Vertex barycentricInterpolation(const Triangle& triangle, const glm::vec3& pos) {
    return barycentricInterpolation(triangle.a, triangle.b, triangle.c, pos);
  }

  static Vertex barycentricInterpolation(const Vertex& a, const Vertex&b, const Vertex& c, const glm::vec3& pos) {
    Vertex result;
    result.position = pos;
    glm::vec3 v0 = b.position - a.position,v1 = c.position - a.position,v2 = pos - a.position;
    float d00 = glm::dot(v0,v0);
    float d01 = glm::dot(v0,v1);
    float d11 = glm::dot(v1,v1);
    float d20 = glm::dot(v2,v0);
    float d21 = glm::dot(v2,v1);
    float inv = 1.f / (d00 * d11 - d01 * d01);
    float v = (d11 * d20 - d01 * d21) * inv;
    float w = (d00 * d21 - d01 * d20) * inv;
    float u = 1.f - v - w;
    result.color = v * a.color + w * b.color + u * c.color;
    return result;
  }
  
  static int intersectTriangle(const Triangle& triangle, const glm::vec3& origin, const glm::vec3& direction, float* out) {
    return intersectTriangle(triangle.a.position, triangle.b.position, triangle.c.position, origin, direction, out);
  }

  static int intersectTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& origin, const glm::vec3& direction, float* out) {
    glm::vec3 edge1, edge2, P, Q, T;
    float det, inv_det, u, v, t;

    edge1 = b - a;
    edge2 = c - a;

    P = glm::cross(direction, edge2);

    det = glm::dot(edge1, P);

    //det near zero: ray in plane with a-b-c
    //not culling?
    if(det > -glm::zero<float>() && det < glm::zero<float>()) return 0;
    inv_det = 1.f / det;

    T = origin - a;

    u = glm::dot(T, P) * inv_det;

    //intersection outside of a-b-c
    if(u < 0.f || u > 1.f) return 0;

    Q = glm::cross(T, edge1);

    v = glm::dot(direction, Q) * inv_det;
    //intersection outside of a-b-c
    if(v < 0.f || u + v> 1.f) return 0;

    t = glm::dot(edge2, Q) * inv_det;

    if(t > glm::zero<float>()) {
      *out = t;
      return 1; 
    }
    return 0;
  }

};
#endif

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <set>
#include <stdexcept>
#include <stack>

#include "Octree.h"
#include "OctreeNode.h"

class Mesh;

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
};

struct Triangle {
  const Mesh* mesh;
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
 public:

      
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

  static bool intersectRayAABox2(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& center, const glm::vec3& halfsize, int& tnear, int& tfar)
  {
    glm::vec3 T_1, T_2, min, max; // vectors to hold the T-values for every direction
    min = center - halfsize;
    max = center + halfsize;
    double t_near = -DBL_MAX; // maximums defined in float.h
    double t_far = DBL_MAX;
   
    for (int i = 0; i < 3; i++){ //we test slabs in every direction
      if (direction[i] == 0){ // ray parallel to planes in this direction
        if ((origin[i] < min[i]) || (origin[i] > max[i])) {
          return false; // parallel AND outside box : no intersection possible
        }
      } else { // ray not parallel to planes in this direction
        T_1[i] = (min[i] - origin[i]) / direction[i];
        T_2[i] = (max[i] - origin[i]) / direction[i];

        if(T_1[i] > T_2[i]){ // we want T_1 to hold values for intersection with near plane
          std::swap(T_1,T_2);
        }
        if (T_1[i] > t_near){
          t_near = T_1[i];
        }
        if (T_2[i] < t_far){
          t_far = T_2[i];
        }
        if( (t_near > t_far) || (t_far < 0) ){
          return false;
        }
      }
    }
    tnear = t_near; tfar = t_far; // put return values in place
    return true; // if we made it here, there was an intersection - YAY
  }

  static void intersectRayOctree2(const Octree& octree, const glm::vec3& origin, const glm::vec3& direction, std::set<Triangle*> *result) {
    std::stack<OctreeNode*> node_stack;
    node_stack.push(&octree.root());
    OctreeNode* current;
    int near = 0, far = 0;
    while(!node_stack.empty()) {
      current = node_stack.top();
      node_stack.pop();
      if(intersectRayAABox2(origin, direction, current->center(), current->halfsize(), near, far)) {
        if(current->leaf()) {
          for(auto triangle : current->triangles()) {
            result->insert(triangle);
          }
        } else {
          for( auto child : current->children()) {
            node_stack.push(child);
          }
        }
      }
    }
  }

};
#endif

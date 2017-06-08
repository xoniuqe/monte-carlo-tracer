#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <set>
#include <stdexcept>
#include <stack>

class Mesh;

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
};

struct Triangle {
  Mesh * mesh;
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
#endif

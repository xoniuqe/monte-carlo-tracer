#ifndef OCTREE_H
#define OCTREE_H

#include <glm/glm.hpp>
//#include "geometry.h" 
#include "OctreeNode.h"

struct Triangle;

class Octree {
 public:
  Octree(int max_triangles);
  ~Octree();
  void insert(Triangle* triangle);
  void subdivide();
  void render() const;
  OctreeNode& root() const;
 private:
  int _max_triangles;
  OctreeNode* _root;
};
#endif

#ifndef OCTREE_H
#define OCTREE_H

#include <glm/glm.hpp>
//#include "geometry.h" 
#include "OctreeNode.h"

struct Triangle;

class Octree {
 public:
  Octree(int maxTrianglesPerNode);
  void insert(Triangle* triangle);
  void subdivide();
  void render() const;
  Triangle* intersectsRay(const glm::vec3& origin, const glm::vec3& dir, float* t_out);
  OctreeNode& root() const;
 private:
  int mMaxTrianglesPerNode;
  OctreeNode* mRoot;
  //glm::vec3[] boundingBox;
};
#endif

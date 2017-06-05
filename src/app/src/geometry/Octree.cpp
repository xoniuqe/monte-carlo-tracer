#include "Octree.h"

#include "geometry.h"

Octree::Octree(int maxTrianglesPerNode) {
  mMaxTrianglesPerNode = maxTrianglesPerNode;
  mRoot = new OctreeNode(maxTrianglesPerNode, glm::vec3(), glm::vec3());
}

void Octree::insert(Triangle* triangle) { 
  mRoot->insert(triangle,true);
  //mRoot->updateBoundingBox();
}

void Octree::subdivide() {
  mRoot->updateBoundingBox();
  mRoot->subdivide();
}

void Octree::render() const {
  mRoot->render();
}

Triangle* Octree::intersectsRay(const glm::vec3& origin, const glm::vec3& dir, float* t_out) {
  Triangle* result = NULL;
  
  return result;
}

OctreeNode& Octree::root() const { return *mRoot;}

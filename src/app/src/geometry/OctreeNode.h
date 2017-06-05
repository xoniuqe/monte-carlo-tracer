#ifndef OCTNODE_H
#define OCTNODE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
//#include "geometry.h"
struct Triangle;

class OctreeNode {
 public:
  OctreeNode(int maxTriangles, const glm::vec3& center, const glm::vec3& halfsize);
  void insert(Triangle* triangle, bool force=false);
  void subdivide();
  int intersectsRay(const glm::vec3& origin, const glm::vec3& direction) const;
  void updateBoundingBox();
  void render() const;
  bool leaf() const;
  glm::vec3 center() const;
  glm::vec3 halfsize() const;
  std::vector<OctreeNode*> children() const;
  std::vector<Triangle*> triangles() const;
 private:
  bool _leaf;
  GLuint vao,vbo;
  std::vector<glm::vec3> verts;
  int mMaxTriangles;
  std::vector<Triangle*> _triangles;
  std::vector<OctreeNode*> mChildren;
  glm::vec3 mBoundingBox[8];
  glm::vec3 mCenter, mHalfsize;
  void setBoundingBox();
}; 

#endif

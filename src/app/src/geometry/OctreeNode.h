#ifndef OCTNODE_H
#define OCTNODE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
//#include "geometry.h"
struct Triangle;

class OctreeNode {
 public:
  OctreeNode(int max_triangles, const glm::vec3& center, const glm::vec3& halfsize);
  ~OctreeNode();
  void insert(Triangle* triangle, bool force=false);
  void subdivide();
  //int intersectsRay(const glm::vec3& origin, const glm::vec3& direction) const;
  void updateBoundingBox();
  void render() const;
  bool leaf() const;
  glm::vec3 center() const;
  glm::vec3 halfsize() const;
  std::vector<OctreeNode*> children() const;
  std::vector<Triangle*> triangles() const;
  void setVAO();
 private:
  bool _leaf;
  GLuint _vao,_vbo;
  std::vector<glm::vec3> _verts;
  int _max_triangles;
  std::vector<Triangle*> _triangles;
  std::vector<OctreeNode*> _children;
  glm::vec3 _bounding_box[8];
  glm::vec3 _center, _halfsize;
  void setBoundingBox();
}; 

#endif

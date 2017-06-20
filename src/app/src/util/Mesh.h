#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#define EPSILON 0.00000001f

//#include "Material.h"
#include "../geometry/geometry.h"

class Scene;

class Mesh {
 public:
  Mesh(const std::vector<Vertex>& verts, const std::vector<GLuint>& indices);
  ~Mesh();
  std::vector<Triangle*> triangles() const;
  void render();
  int material() const;
  void material(int material);
 private:
  int _material;
  glm::vec3 _center;
  GLuint _vao, _vbo, _ibo;
  std::vector<Vertex> _vertices;
  std::vector<GLuint> _indices;
};

#endif

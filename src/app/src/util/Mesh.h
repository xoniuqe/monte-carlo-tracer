#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#define EPSILON 0.00000001f

class Scene;

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
};

class Mesh {
 public:
  Mesh(const std::vector<Vertex>& verts, const std::vector<GLuint>& indices);
  ~Mesh();
  int intersect(const glm::vec3& origin, const glm::vec3& direction, float* out_t, glm::vec3* out_n);
  void render();
  int intersectTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& origin, const glm::vec3& direction, float* out);
 private:
  glm::vec3 mCenter;
  GLuint mVertexArrayId;
  GLuint mVertexBufferId;
  GLuint mIndexBufferId;
  std::vector<Vertex> mVertices;
  std::vector<GLuint> mIndices;
};

#endif

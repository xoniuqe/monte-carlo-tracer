#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#define EPSILON 0.00000001f

#include "../geometry/geometry.h"

class Scene;

class Mesh {
 public:
  Mesh(const std::vector<Vertex>& verts, const std::vector<GLuint>& indices);
  ~Mesh();
  int intersect(const glm::vec3& origin, const glm::vec3& direction, float* out_t, Vertex* out);//glm::vec3* out_n);
  std::vector<Triangle*> triangles() const;
  void render();
 private:
  static Vertex barcentricInterpolation(const Vertex& a, const Vertex&b, const Vertex& c, const glm::vec3& p);
  static int intersectTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& origin, const glm::vec3& direction, float* out);
  glm::vec3 mCenter;
  GLuint mVertexArrayId;
  GLuint mVertexBufferId;
  GLuint mIndexBufferId;
  std::vector<Vertex> mVertices;
  std::vector<GLuint> mIndices;
};

#endif

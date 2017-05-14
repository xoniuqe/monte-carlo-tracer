#include "Mesh.h"
#include "../Scene.h"

#include <cmath>
#include <iostream>

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<GLuint>& indices) {
  mVertices = verts;
  mIndices = indices;


  glGenVertexArrays(1, &mVertexArrayId);
  glGenBuffers(1, &mVertexBufferId);
  glGenBuffers(1, &mIndexBufferId);

  glBindVertexArray(mVertexArrayId);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vertex) * mVertices.size(),
               &mVertices.at(0),
               GL_STATIC_DRAW);


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(GLuint) * mIndices.size(),
               &mIndices.at(0),
               GL_STATIC_DRAW);


  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (3 * sizeof(float)));
  glEnableVertexAttribArray(2);
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (6 * sizeof(float)));
  glBindVertexArray(0);
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &mVertexArrayId); 
}

void Mesh::render() {
  glBindVertexArray(mVertexArrayId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
  //glDrawElements(GL_QUADS, mIndices.size(), GL_UNSIGNED_INT, 0);
  glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

int Mesh::intersect(const glm::vec3& origin, const glm::vec3& direction, float* out) {
  int result = 0;
  float tmp = 0.0;
  for(int i = 0; i < mIndices.size(); i += 3) {
    Vertex a = mVertices.at(mIndices.at(i));
    Vertex b = mVertices.at(mIndices.at(i+1));
    Vertex c = mVertices.at(mIndices.at(i+2));
    
    int intersection = intersectTriangle(a.position, b.position, c.position, origin, direction, &tmp);
    if(!result && intersection) {
      *out = tmp;
    } else {
      //Get closest intersection
    }
    if(intersection) {
      result = 1;
      glm::vec3 p = origin + direction * tmp;
    }
  }
  return result;
}

int Mesh::intersectTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& origin, const glm::vec3& direction, float* out) {
  glm::vec3 edge1, edge2, P, Q, T;
  float det, inv_det, u, v, t;

  edge1 = b - a;
  edge2 = c - a;

  P = glm::cross(direction, edge2);

  det = glm::dot(edge1, P);
  //det near zero: ray in plane with a-b-c
  //not culling?
  if(det > -EPSILON && det > EPSILON) return 0;
  inv_det = 1.f / det;

  T = origin - a;

  u = glm::dot(T, P) * inv_det;

  //intersection outside of a-b-c
  if(u < 0.f || u > 1.f) return 0;

  Q = glm::cross(T, edge1);

  v = glm::dot(direction, Q) * inv_det;
  //intersection outside of a-b-c
  if(v < 0.f || u > 1.f) return 0;

  t = glm::dot(edge2, Q) * inv_det;

  if(t > EPSILON) {
    *out = t;
    return 1; 
  }
  return 0;
}

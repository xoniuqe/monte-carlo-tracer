#include "Mesh.h"
#include "../Scene.h"

#include <cmath>
#include <iostream>
#include <glm/gtc/constants.hpp>

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<GLuint>& indices) {
  mVertices = verts;
  mIndices = indices;
  _material = 0;


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

int Mesh::material() const {
  return _material;
}

void Mesh::material(int material) {
    _material = material;
}

void Mesh::render() {
  glBindVertexArray(mVertexArrayId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
  //glDrawElements(GL_QUADS, mIndices.size(), GL_UNSIGNED_INT, 0);
  glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

std::vector<Triangle*> Mesh::triangles() const {
  std::vector<Triangle*> triangles;
  for(int i = 0; i < mIndices.size(); i += 3) {
    Vertex a = mVertices.at(mIndices.at(i));
    Vertex b = mVertices.at(mIndices.at(i+1));
    Vertex c = mVertices.at(mIndices.at(i+2));
    Triangle* triangle = new Triangle();
    //triangle->mesh = this;
    triangle->a = a;
    triangle->b = b;
    triangle->c = c;
    triangles.push_back(triangle);
  }
  return triangles;
}

int Mesh::intersect(const glm::vec3& origin, const glm::vec3& direction, float* out_t, Vertex* out) { //glm::vec3* out_n) {
  int result = 0;
  float tmp = 0.0;
  float best = glm::zero<float>();
  Vertex res;
  glm::vec3 n;
  for(int i = 0; i < mIndices.size(); i += 3) {
    Vertex a = mVertices.at(mIndices.at(i));
    Vertex b = mVertices.at(mIndices.at(i+1));
    Vertex c = mVertices.at(mIndices.at(i+2));

    glm::vec3 tmpn = glm::normalize(a.normal + b.normal + c.normal);
    int intersection = Mesh::intersectTriangle(a.position, b.position, c.position, origin, direction, &tmp);
    if((!result || tmp < best) && intersection && tmp > 0.00001f && glm::dot(tmpn, direction) <= glm::zero<float>()) {
      best = tmp;
      n = glm::normalize(a.normal + b.normal + c.normal);
      result = 1;
      res = Mesh::barcentricInterpolation(a,b,c, origin + direction * tmp);
      res.normal = glm::normalize(a.normal + b.normal + c.normal);
    }
  }
  *out_t = best;
  //  *out_n = n;
  *out = res;
  return result;
}

Vertex Mesh::barcentricInterpolation(const Vertex& a, const Vertex&b, const Vertex& c, const glm::vec3& pos) {
  Vertex result;
  result.position = pos;
  glm::vec3 v0 = b.position - a.position,v1 = c.position - a.position,v2 = pos - a.position;
  float d00 = glm::dot(v0,v0);
  float d01 = glm::dot(v0,v1);
  float d11 = glm::dot(v1,v1);
  float d20 = glm::dot(v2,v0);
  float d21 = glm::dot(v2,v1);
  float inv = 1.f / (d00 * d11 - d01 * d01);
  float v = (d11 * d20 - d01 * d21) * inv;
  float w = (d00 * d21 - d01 * d20) * inv;
  float u = 1.f - v - w;
  result.color = v * a.color + w * b.color + u * c.color;
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
  if(det > -glm::zero<float>() && det < glm::zero<float>()) return 0;
  inv_det = 1.f / det;

  T = origin - a;

  u = glm::dot(T, P) * inv_det;

  //intersection outside of a-b-c
  if(u < 0.f || u > 1.f) return 0;

  Q = glm::cross(T, edge1);

  v = glm::dot(direction, Q) * inv_det;
  //intersection outside of a-b-c
  if(v < 0.f || u + v> 1.f) return 0;

  t = glm::dot(edge2, Q) * inv_det;

  if(t > glm::zero<float>()) {
    *out = t;
    return 1; 
  }
  return 0;
}

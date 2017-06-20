#include "Mesh.h"
#include "../Scene.h"
#include "../geometry/Intersection.h"

#include <cmath>
#include <iostream>
#include <glm/gtc/constants.hpp>

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<GLuint>& indices) {
  _vertices = verts;
  _indices = indices;
  _material = 0;


  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ibo);

  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vertex) * _vertices.size(),
               &_vertices.at(0),
               GL_STATIC_DRAW);


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(GLuint) * _indices.size(),
               &_indices.at(0),
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
  glDeleteVertexArrays(1, &_vao); 
}

int Mesh::material() const {
  return _material;
}

void Mesh::material(int material) {
    _material = material;
}

void Mesh::render() {
    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

std::vector<Triangle*> Mesh::triangles() const {
  std::vector<Triangle*> triangles;
  for(int i = 0; i < _indices.size(); i += 3) {
    Vertex a = _vertices.at(_indices.at(i));
    Vertex b = _vertices.at(_indices.at(i+1));
    Vertex c = _vertices.at(_indices.at(i+2));
    Triangle* triangle = new Triangle();
    triangle->a = a;
    triangle->b = b;
    triangle->c = c;
    triangles.push_back(triangle);
  }
  return std::move(triangles);
}

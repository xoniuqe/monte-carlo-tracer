#include "Octree.h"

#include "geometry.h"

Octree::Octree(int max_triangles) {
    _max_triangles = max_triangles;
    _root = new OctreeNode(_max_triangles, glm::vec3(), glm::vec3());
}

Octree::~Octree() {
    delete _root;
}

void Octree::insert(Triangle* triangle) { 
  _root->insert(triangle,true);
}

void Octree::subdivide() {
  _root->updateBoundingBox();
  _root->subdivide();
  _root->setVAO();
}

void Octree::render() const {
  _root->render();
}

OctreeNode& Octree::root() const { return *_root;}

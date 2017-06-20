#include "OctreeNode.h"
#include "geometry.h"
#include "Intersection.h"

#include <iostream>

OctreeNode::OctreeNode(int max_triangles, const glm::vec3& center, const glm::vec3& halfsize) {
    _max_triangles = max_triangles;
    _center = center;
    _halfsize = halfsize;
    _leaf = true;
    setBoundingBox();
    
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    setVAO();
}

OctreeNode::~OctreeNode() {
    for(auto child : _children) {
        delete child;
    }
    _children.clear();
    for(auto triangle : _triangles) {
        delete triangle;
    }
    _triangles.clear();
}

void OctreeNode::setVAO() {
    _verts.clear();
    _verts.push_back(_bounding_box[0]);
    _verts.push_back(_bounding_box[1]);
    _verts.push_back(_bounding_box[0]);
    _verts.push_back(_bounding_box[2]);
    _verts.push_back(_bounding_box[1]);
    _verts.push_back(_bounding_box[3]);
    _verts.push_back(_bounding_box[2]);
    _verts.push_back(_bounding_box[3]);
  
    _verts.push_back(_bounding_box[0]);
    _verts.push_back(_bounding_box[4]);
    _verts.push_back(_bounding_box[1]);
    _verts.push_back(_bounding_box[5]);
    _verts.push_back(_bounding_box[2]);
    _verts.push_back(_bounding_box[6]);
    _verts.push_back(_bounding_box[3]);
    _verts.push_back(_bounding_box[7]);
    
    _verts.push_back(_bounding_box[4]);
    _verts.push_back(_bounding_box[5]);
    _verts.push_back(_bounding_box[4]);
    _verts.push_back(_bounding_box[6]);
    _verts.push_back(_bounding_box[6]);
    _verts.push_back(_bounding_box[7]);
    _verts.push_back(_bounding_box[5]);
    _verts.push_back(_bounding_box[7]);
    
    
    glBindVertexArray(_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER,
               sizeof(glm::vec3) * sizeof(_verts),
                 &_verts.at(0),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    
    glBindVertexArray(0); 
}
void OctreeNode::insert(Triangle* triangle, bool force) {
    if(force || Intersection::triangleBoxOverlap(_center,_halfsize, *triangle)) {
        _triangles.push_back(triangle);
    }
}

bool OctreeNode::leaf() const { return _leaf; }
glm::vec3 OctreeNode::center() const { return _center; }
glm::vec3 OctreeNode::halfsize() const { return _halfsize; }
std::vector<OctreeNode*> OctreeNode::children() const { return _children; }
std::vector<Triangle*> OctreeNode::triangles() const { return _triangles; }

void OctreeNode::subdivide() {
  if(_triangles.size() > _max_triangles) {
    _leaf = false;
    glm::vec3 quatersize = _halfsize * 0.5f;
    OctreeNode *n0 = new OctreeNode(_max_triangles, _center - quatersize, quatersize);
    OctreeNode *n1 = new OctreeNode(_max_triangles, glm::vec3(_center.x - quatersize.x, _center.y - quatersize.y, _center.z + quatersize.z), quatersize);
    OctreeNode *n2 = new OctreeNode(_max_triangles, glm::vec3(_center.x - quatersize.x, _center.y + quatersize.y, _center.z - quatersize.z), quatersize);
    OctreeNode *n3 = new OctreeNode(_max_triangles, glm::vec3(_center.x - quatersize.x, _center.y + quatersize.y, _center.z + quatersize.z), quatersize);
    OctreeNode *n4 = new OctreeNode(_max_triangles, glm::vec3(_center.x + quatersize.x, _center.y - quatersize.y, _center.z - quatersize.z), quatersize);
    OctreeNode *n5 = new OctreeNode(_max_triangles, glm::vec3(_center.x + quatersize.x, _center.y - quatersize.y, _center.z + quatersize.z), quatersize);
    OctreeNode *n6 = new OctreeNode(_max_triangles, glm::vec3(_center.x + quatersize.x, _center.y + quatersize.y, _center.z - quatersize.z), quatersize);
    OctreeNode *n7 = new OctreeNode(_max_triangles, _center + quatersize, quatersize);
    _children.push_back(n0);
    _children.push_back(n1);
    _children.push_back(n2);
    _children.push_back(n3);
    _children.push_back(n4);
    _children.push_back(n5);
    _children.push_back(n6);
    _children.push_back(n7);

    for(auto child : _children) {
      for(auto triangle : _triangles) {
        child->insert(triangle);
      }
      child->subdivide();
    }
    _triangles.clear();
  }
}


void OctreeNode::setBoundingBox() {
  _bounding_box[0] = _center - _halfsize;
  _bounding_box[7] = _center + _halfsize;
  glm::vec3 xOff((_bounding_box[7].x - _bounding_box[0].x), 0, 0);
  glm::vec3 yOff(0, (_bounding_box[7].y - _bounding_box[0].y), 0);
  glm::vec3 zOff(0, 0, (_bounding_box[7].z - _bounding_box[0].z));
  _bounding_box[1] = _bounding_box[0] + xOff;
  _bounding_box[2] = _bounding_box[0] + zOff;
  _bounding_box[3] = _bounding_box[0] + xOff + zOff;
  _bounding_box[4] = _bounding_box[0] + yOff;
  _bounding_box[5] = _bounding_box[0] + xOff + yOff;
  _bounding_box[6] = _bounding_box[0] + zOff + yOff;
  //rest berechnen
}

void OctreeNode::updateBoundingBox() {
  std::cout << "update\n";
  glm::vec3 min = (*_triangles[0]).a.position, max = (*_triangles[0]).a.position;
  for(auto triangle : _triangles) {
    for(auto i = 0; i <= 2; i++) {
      
      glm::vec3 pos = (*triangle)[i].position;
      for(auto dim = 0; dim <= 2; dim++) {
        if(min[dim] > pos[dim])
          min[dim] = pos[dim];
        if(max[dim] < pos[dim])
          max[dim] = pos[dim];
      }
    }
  }
  _center = (max + min) * 0.5f;
  _halfsize = max - _center;
  std::cout << "_center: " << _center.x << " " << _center.y << " " << _center.z << "\n";
  std::cout << "halfsize: " << _halfsize.x << " " << _halfsize.y << " " << _halfsize.z << "\n";
  setBoundingBox();
}

void OctreeNode::render() const {
  glBindVertexArray(_vao);
  glDrawArrays(GL_LINES, 0, _verts.size());
  glBindVertexArray(0);
  for(auto child : _children) {
    child->render();
  }
}

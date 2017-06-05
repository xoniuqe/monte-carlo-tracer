#include "OctreeNode.h"
#include "geometry.h"

#include <iostream>

OctreeNode::OctreeNode(int maxTriangles, const glm::vec3& center, const glm::vec3& halfsize) {
  this->mMaxTriangles = maxTriangles;
  mCenter = center;
  mHalfsize = halfsize;
  _leaf = true;
  setBoundingBox();

  verts.push_back(mBoundingBox[0]);
  verts.push_back(mBoundingBox[1]);
  verts.push_back(mBoundingBox[0]);
  verts.push_back(mBoundingBox[2]);
  verts.push_back(mBoundingBox[1]);
  verts.push_back(mBoundingBox[3]);
  verts.push_back(mBoundingBox[2]);
  verts.push_back(mBoundingBox[3]);

  verts.push_back(mBoundingBox[0]);
  verts.push_back(mBoundingBox[4]);
  verts.push_back(mBoundingBox[1]);
  verts.push_back(mBoundingBox[5]);
  verts.push_back(mBoundingBox[2]);
  verts.push_back(mBoundingBox[6]);
  verts.push_back(mBoundingBox[3]);
  verts.push_back(mBoundingBox[7]);

  verts.push_back(mBoundingBox[4]);
  verts.push_back(mBoundingBox[5]);
  verts.push_back(mBoundingBox[4]);
  verts.push_back(mBoundingBox[6]);
  verts.push_back(mBoundingBox[6]);
  verts.push_back(mBoundingBox[7]);
  verts.push_back(mBoundingBox[5]);
  verts.push_back(mBoundingBox[7]);
  
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glm::vec3) * sizeof(verts),
               &verts.at(0),
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
 
  glBindVertexArray(0);
}

void OctreeNode::insert(Triangle* triangle, bool force) {
  if(force || Intersection::triangleBoxOverlap(mCenter,mHalfsize, *triangle)) {
    _triangles.push_back(triangle);
  }
}

bool OctreeNode::leaf() const { return _leaf; }
glm::vec3 OctreeNode::center() const { return mCenter; }
glm::vec3 OctreeNode::halfsize() const { return mHalfsize; }
std::vector<OctreeNode*> OctreeNode::children() const { return mChildren; }
std::vector<Triangle*> OctreeNode::triangles() const { return _triangles; }

void OctreeNode::subdivide() {
  if(_triangles.size() > mMaxTriangles) {
    _leaf = false;
    glm::vec3 quatersize = mHalfsize * 0.5f;
    OctreeNode *n0 = new OctreeNode(mMaxTriangles, mCenter - quatersize, quatersize);
    OctreeNode *n1 = new OctreeNode(mMaxTriangles, glm::vec3(mCenter.x - quatersize.x, mCenter.y - quatersize.y, mCenter.z + quatersize.z), quatersize);
    OctreeNode *n2 = new OctreeNode(mMaxTriangles, glm::vec3(mCenter.x - quatersize.x, mCenter.y + quatersize.y, mCenter.z - quatersize.z), quatersize);
    OctreeNode *n3 = new OctreeNode(mMaxTriangles, glm::vec3(mCenter.x - quatersize.x, mCenter.y + quatersize.y, mCenter.z + quatersize.z), quatersize);
    OctreeNode *n4 = new OctreeNode(mMaxTriangles, glm::vec3(mCenter.x + quatersize.x, mCenter.y - quatersize.y, mCenter.z - quatersize.z), quatersize);
    OctreeNode *n5 = new OctreeNode(mMaxTriangles, glm::vec3(mCenter.x + quatersize.x, mCenter.y - quatersize.y, mCenter.z + quatersize.z), quatersize);
    OctreeNode *n6 = new OctreeNode(mMaxTriangles, glm::vec3(mCenter.x + quatersize.x, mCenter.y + quatersize.y, mCenter.z - quatersize.z), quatersize);
    OctreeNode *n7 = new OctreeNode(mMaxTriangles, mCenter + quatersize, quatersize);
    mChildren.push_back(n0);
    mChildren.push_back(n1);
    mChildren.push_back(n2);
    mChildren.push_back(n3);
    mChildren.push_back(n4);
    mChildren.push_back(n5);
    mChildren.push_back(n6);
    mChildren.push_back(n7);

    for(auto child : mChildren) {
      for(auto triangle : _triangles) {
        child->insert(triangle);
      }
      child->subdivide();
    }
  }
}


void OctreeNode::setBoundingBox() {
  mBoundingBox[0] = mCenter - mHalfsize;
  mBoundingBox[7] = mCenter + mHalfsize;
  glm::vec3 xOff((mBoundingBox[7].x - mBoundingBox[0].x), 0, 0);
  glm::vec3 yOff(0, (mBoundingBox[7].y - mBoundingBox[0].y), 0);
  glm::vec3 zOff(0, 0, (mBoundingBox[7].z - mBoundingBox[0].z));
  mBoundingBox[1] = mBoundingBox[0] + xOff;
  mBoundingBox[2] = mBoundingBox[0] + zOff;
  mBoundingBox[3] = mBoundingBox[0] + xOff + zOff;
  mBoundingBox[4] = mBoundingBox[0] + yOff;
  mBoundingBox[5] = mBoundingBox[0] + xOff + yOff;
  mBoundingBox[6] = mBoundingBox[0] + zOff + yOff;
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
  mCenter = (max + min) * 0.5f;
  mHalfsize = max - mCenter;
  std::cout << "mCenter: " << mCenter.x << " " << mCenter.y << " " << mCenter.z << "\n";
  std::cout << "halfsize: " << mHalfsize.x << " " << mHalfsize.y << " " << mHalfsize.z << "\n";
  setBoundingBox();
}

void OctreeNode::render() const {
  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, verts.size());
  glBindVertexArray(0);
  for(auto child : mChildren) {
    child->render();
  }
}

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "util\Shape.h"

struct Intersection {
  Intersection(Vector3 intersectionPoint, Vector3 normal) { this->intersectionPoint = intersectionPoint; this->normal = normal; }
  Vector3 intersectionPoint;
  Vector3 normal;

};

class Scene {
 public:
  void addShape(Shape* shape);
  Intersection& intersection(const Vector3& origin, const Vector3& dir) const;

 private:
  std::vector<Shape*> objects;

};

#endif

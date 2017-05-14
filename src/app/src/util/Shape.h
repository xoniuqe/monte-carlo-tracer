#ifndef SHAPE_H
#define SHAPE_H

#include "Scene.h"
#include "Vector3.h"

class Shape {
 public:
  virtual Vector3 normal(const Vector3& direction) = 0;
  virtual Intersection intersect(const Vector3& origin, const Vector3& direction) = 0;
  virtual Vector3~() = default;
 private:
  Vector3 center;
};

#endif

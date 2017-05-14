#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

class Sphere : Shape {
 public:
  Sphere(Vector3 center, float r);
  Vector3 normal(const Vector3& direction) const;
  Intersection intersect(const Vector3& origin, const Vector3& direction) const;
 private:
  float r;
 
};

#endif

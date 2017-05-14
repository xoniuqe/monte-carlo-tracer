#include "Sphere.h"

Vector3 Sphere::normal(const Vector3& direction) const {
  Vector3 n = this->center - direction;
  n.normalize();
  n *= this->r;
  return n;
}

Intersection Sphere::intersect(const Vector3& origin, const Vector3& direction) const {
  Vector3 dDO(direction.x, direction.y, direction.z);
  dDO -= origin;
  Vector3 dDC(direction.x, direction.y, direction.z);
  dDC -= center;

  float a = dxBxA + dyByA + dzBzA;
  float b = 2* ()
  float c = 

}

#include <cmath>

#include "Vector3.h"

//Vector3::Vector3() {
//  this(0,0,0);
//}

Vector3::Vector3(const float x, const float y, const float z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

float Vector3::length() const {
  return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

Vector3& Vector3::normalize() {
  float lenInv = 1.0 / this->length();
  this->x *= lenInv;
  this->y *= lenInv;
  this->z *= lenInv;
  return *this;
}

Vector3& Vector3::operator+=(const Vector3& rhs) {
  this->x += rhs.x;
  this->y += rhs.y;
  this->z += rhs.z;
  return *this;
}

Vector3& Vector3::operator*=(const float scalar) {
  this->x *= scalar;
  this->y *= scalar;
  this->z *= scalar;
  return *this;
}

Vector3& Vector3::operator-=(const Vector3& rhs) {
  this->x -= rhs.x;
  this->y -= rhs.y;
  this->z -= rhs.z;
  return *this;
}

float Vector3::dot(const Vector3& rhs) const {
  return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
}

Vector3& Vector3::cross(const Vector3& rhs) {
  float x,y,z;
  x = this->y * rhs.z - this->z * rhs.y;
  y = this->z * rhs.x - this->x * rhs.z;
  z = this->x * rhs.y - this->y * rhs.x;
  this->x = x;
  this->y = y;
  this->z = z;
  return *this;
}

float Vector3::angle(const Vector3& a, const Vector3& b) {
  float dot = a.dot(b);
  float len_a = a.length(), len_b = b.length();
  return dot / (len_a * len_b);
}

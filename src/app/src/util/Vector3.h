
#ifndef VECTOR3_H
#define VECTOR3_H

class Vector3 {
 public:
  //Vector3();
  Vector3(const float x, const float y, const float z);
  float x,y,z;

  float length() const;

  Vector3& normalize();
 
  Vector3& operator+=(const Vector3& rhs);

  Vector3& operator*=(const float scalar);

  Vector3& operator-=(const Vector3& rhs);

  float dot(const Vector3& rhs) const;

  Vector3& cross(const Vector3& rhs);

  static float angle(const Vector3& a, const Vector3& b);
 private:



};

#endif

#ifndef MATERIAL_H
#define MATERIAL_H

#include <map>

struct Material {
  static std::map<int,Material*> materials;
  Material(int index) {
    Material::materials[index] = this;
  }
  float shininess = 0.f;
  float refraction_index = 0.f;
  float reflectivity = 0.f;
};

#endif

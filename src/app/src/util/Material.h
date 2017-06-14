#ifndef MATERIAL_H
#define MATERIAL_H

#include <map>
#include <glm/glm.hpp>

struct Material {
    static std::map<int,Material*> materials;
    static int material_count;
    static Material* new_material();

    Material(int _index);
    glm::vec3 diffuse, specular, emitting;
    float shininess = 0.f;
    float refraction_index = 0.f;
    float reflectivity = 0.f;
    const int index;
};

#endif

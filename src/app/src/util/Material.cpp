#include "Material.h"

std::map<int, Material*> Material::materials = {};
int Material::material_count = 0;

Material* Material::new_material() {
    Material* nm = new Material(material_count);
    materials[material_count++] = nm;
    return nm;
}

Material::Material(int _index) : index(_index) {
}

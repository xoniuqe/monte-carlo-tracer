#include "Material.h"

std::map<int, Material*> Material::materials = {};

Material::Material(int index) {
    if(!Material::materials[index]) {
        Material::materials[index] = this;
    }
}

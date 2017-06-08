#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "geometry.h"
#include "Octree.h"
#include <glm/glm.hpp>

class Intersection {
  private:
    static bool axis_test_x(const glm::vec3& eX, const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& halfsize);
    static bool axis_test_y(const glm::vec3& eX, const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& halfsize);
    static bool axis_test_z(const glm::vec3& eX, const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& halfsize);
    static void findMinMax(const float& a, const float& b, const float& c, float* _min, float* _max);
    static int planeBoxOverlap(const glm::vec3& normal, const glm::vec3& vertex, const glm::vec3& maxbox);
  public:    
    static int triangleBoxOverlap(const glm::vec3& center, const glm::vec3& halfsize, const Triangle& triangle);
    static Vertex barycentricInterpolation(const Triangle& triangle, const glm::vec3& pos);
    static Vertex barycentricInterpolation(const Vertex& a, const Vertex&b, const Vertex& c, const glm::vec3& pos);
    static int intersectTriangle(const Triangle& triangle, const glm::vec3& origin, const glm::vec3& direction, float* out);
    static int intersectTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& origin, const glm::vec3& direction, float* out);
    static bool intersectRayAABox2(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& center, const glm::vec3& halfsize, int& tnear, int& tfar);
    static void intersectRayOctree(const Octree& octree, const glm::vec3& origin, const glm::vec3& direction, std::set<Triangle*> *result);
};
#endif

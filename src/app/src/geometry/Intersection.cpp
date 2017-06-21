#include "Intersection.h"

#include "OctreeNode.h"

bool Intersection::axis_test_x(const glm::vec3& eX, const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& halfsize) {
    float pA = eX.z * vA.y - eX.y * vA.z;
    float pB = eX.z * vB.y - eX.y * vB.z;
    float rad = fabs(eX.z) * halfsize.y + fabs(eX.y) * halfsize.z;
    return (fmin(pA,pB) > rad || fmax(pA,pB) < -rad);
}

bool Intersection::axis_test_y(const glm::vec3& eX, const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& halfsize) {
    float pA = - eX.z * vA.x + eX.x * vA.z;
    float pB = - eX.z * vB.x + eX.x * vB.z;
    float rad = fabs(eX.z) * halfsize.x + fabs(eX.x) * halfsize.z;
    return (fmin(pA,pB) > rad || fmax(pA,pB) < -rad);
}

bool Intersection::axis_test_z(const glm::vec3& eX, const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& halfsize) {
    float pA = eX.y * vA.x - eX.x * vA.y;
    float pB = eX.y * vB.x - eX.x * vB.y;
    float rad = fabs(eX.y) * halfsize.x + fabs(eX.x) * halfsize.y;
    return (fmin(pA,pB) > rad || fmax(pA,pB) < -rad);
}

void Intersection::findMinMax(const float& a, const float& b, const float& c, float* _min, float* _max) {
    *_min = *_max = a;
    if(b < *_min) *_min = b;
    if(c < *_min) *_min = c;
    if(b > *_max) *_max = b;
    if(c > *_max) *_max = c;
}

int Intersection::planeBoxOverlap(const glm::vec3& normal, const glm::vec3& vertex, const glm::vec3& maxbox) {
    glm::vec3 _min,_max;
    for(auto dim = 0; dim <= 2; dim++) {
        if(normal[dim] > glm::zero<float>()) {
            _min[dim] = -maxbox[dim] - vertex[dim];
            _max[dim] = maxbox[dim] - vertex[dim];
        } else {
            _min[dim] = maxbox[dim] - vertex[dim];
            _max[dim] = -maxbox[dim] - vertex[dim];
        }
    }
    if(glm::dot(normal,_min) > glm::zero<float>()) return 0;
    if(glm::dot(normal,_max) >= glm::zero<float>()) return 1;
    return 0;
} 

int Intersection::triangleBoxOverlap(const glm::vec3& center, const glm::vec3& halfsize, const Triangle& triangle) {
    glm::vec3 v0,v1,v2, normal, e0, e1, e2;
    float _min,_max;
     
    v0 = triangle.a.position - center;
    v1 = triangle.b.position - center;
    v2 = triangle.c.position - center;
  
    e0 = v1 - v0;
    e1 = v2 - v1;
    e2 = v0 - v2;

    //AXISTESTS
    if(axis_test_x(e0,v0,v2,halfsize)) return 0;
    if(axis_test_y(e0,v0,v2,halfsize)) return 0;
    if(axis_test_z(e0,v1,v2,halfsize)) return 0;

    if(axis_test_x(e1,v0,v2,halfsize)) return 0;
    if(axis_test_y(e1,v0,v2,halfsize)) return 0;
    if(axis_test_z(e1,v0,v1,halfsize)) return 0;

    if(axis_test_x(e2,v0,v1,halfsize)) return 0;
    if(axis_test_y(e2,v0,v1,halfsize)) return 0;
    if(axis_test_z(e2,v1,v2,halfsize)) return 0; 

    findMinMax(v0.x,v1.x,v2.x,&_min,&_max);
    if(_min > halfsize.x || _max < -halfsize.x) return 0;

    findMinMax(v0.y,v1.y,v2.y,&_min,&_max);
    if(_min > halfsize.y || _max < -halfsize.y) return 0;

    findMinMax(v0.z,v1.z,v2.z,&_min,&_max);
    if(_min > halfsize.z || _max < -halfsize.z) return 0;

    normal = glm::cross(e0,e1);

    if(!planeBoxOverlap(normal,v0,halfsize)) return 0;

    return 1;
}
      
Vertex Intersection::barycentricInterpolation(const Triangle& triangle, const glm::vec3& pos) {
    return barycentricInterpolation(triangle.a, triangle.b, triangle.c, pos);
}

Vertex Intersection::barycentricInterpolation(const Vertex& a, const Vertex&b, const Vertex& c, const glm::vec3& pos) {
    Vertex result;
    result.position = pos;
    glm::vec3 v0 = b.position - a.position,v1 = c.position - a.position,v2 = pos - a.position;
    float d00 = glm::dot(v0,v0);
    float d01 = glm::dot(v0,v1);
    float d11 = glm::dot(v1,v1);
    float d20 = glm::dot(v2,v0);
    float d21 = glm::dot(v2,v1);
    float inv = 1.f / (d00 * d11 - d01 * d01);
    float alpha = (d11 * d20 - d01 * d21) * inv;
    float beta = (d00 * d21 - d01 * d20) * inv;
    float gamma = 1.f - alpha - beta;
    result.color = alpha * a.color + beta * b.color + gamma * c.color;
    result.normal = alpha * a.normal + beta * b.normal + gamma * c.normal;
    return result;
}
  
int Intersection::intersectTriangle(const Triangle& triangle, const glm::vec3& origin, const glm::vec3& direction, float* out) {
    return intersectTriangle(triangle.a.position, triangle.b.position, triangle.c.position, origin, direction, out);
}

int Intersection::intersectTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& origin, const glm::vec3& direction, float* out) {
    glm::vec3 edge1, edge2, P, Q, T;
    float det, inv_det, u, v, t;

    edge1 = b - a;
    edge2 = c - a;

    P = glm::cross(direction, edge2);

    det = glm::dot(edge1, P);

    //det near zero: ray in plane with a-b-c
    //not culling?
    if(det > -glm::zero<float>() && det < glm::zero<float>()) return 0;
    inv_det = 1.f / det;

    T = origin - a;

    u = glm::dot(T, P) * inv_det;

    //intersection outside of a-b-c
    if(u < 0.f || u > 1.f) return 0;

    Q = glm::cross(T, edge1);

    v = glm::dot(direction, Q) * inv_det;
    //intersection outside of a-b-c
    if(v < 0.f || u + v> 1.f) return 0;

    t = glm::dot(edge2, Q) * inv_det;

    if(t > glm::zero<float>()) {
        *out = t;
        return 1; 
    }
    return 0;
}

bool Intersection::intersectRayAABox2(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& center, const glm::vec3& halfsize, int& tnear, int& tfar)
{
    glm::vec3 T_1, T_2, min, max; // vectors to hold the T-values for every direction
    min = center - halfsize;
    max = center + halfsize;
    double t_near = -DBL_MAX; // maximums defined in float.h
    double t_far = DBL_MAX;
   
    for (int i = 0; i < 3; i++){ //we test slabs in every direction
        if (direction[i] == 0){ // ray parallel to planes in this direction
            if ((origin[i] < min[i]) || (origin[i] > max[i])) {
                return false; // parallel AND outside box : no intersection possible
            }
        } else { // ray not parallel to planes in this direction
            T_1[i] = (min[i] - origin[i]) / direction[i];
            T_2[i] = (max[i] - origin[i]) / direction[i];

            if(T_1[i] > T_2[i]){ // we want T_1 to hold values for intersection with near plane
                std::swap(T_1,T_2);
            }
            if (T_1[i] > t_near){
                t_near = T_1[i];
            }
            if (T_2[i] < t_far){
                t_far = T_2[i];
            }
            if( (t_near > t_far) || (t_far < 0) ){
                return false;
            }
        }
    }
    tnear = t_near; tfar = t_far; // put return values in place
    return true; // if we made it here, there was an intersection - YAY
}

void Intersection::intersectRayOctree(const Octree& octree, const glm::vec3& origin, const glm::vec3& direction, std::set<Triangle*> *result) {
    std::stack<OctreeNode*> node_stack;
    node_stack.push(&octree.root());
    OctreeNode* current;
    int near = 0, far = 0;
    while(!node_stack.empty()) {
        current = node_stack.top();
        node_stack.pop();
        if(intersectRayAABox2(origin, direction, current->center(), current->halfsize(), near, far)) {
            if(current->leaf()) {
                for(auto triangle : current->triangles()) {
                    result->insert(triangle);
                }
            } else {
                for( auto child : current->children()) {
                    node_stack.push(child);
                }
            }
        }
    }
}

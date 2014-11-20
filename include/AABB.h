#include <glm/glm.hpp>

class AABB {
public:
    glm::vec3 origin; // lower left corner (or so^^)
    glm::vec3 radii; // width, height, depth (or so^^)
};
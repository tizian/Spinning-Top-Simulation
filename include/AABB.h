#include <glm/glm.hpp>

class AABB {
public:
    glm::vec3 origin; // lower left corner
    glm::vec3 radii; // width, height, depth
    
    GLuint getNumVertices() {
        return 3*8; // To be consistent with Mesh.cpp
    }
    
    GLfloat * getVertices() {
        if (vertices == nullptr)
        {
            vertices = new GLfloat[3*8];
        
            vertices[0] = origin.x;
            vertices[1] = origin.y;
            vertices[2] = origin.z;
            
            vertices[3] = origin.x + radii.x;
            vertices[4] = origin.y;
            vertices[5] = origin.z;
            
            vertices[6] = origin.x;
            vertices[7] = origin.y + radii.y;
            vertices[8] = origin.z;
            
            vertices[9] = origin.x;
            vertices[10] = origin.y;
            vertices[11] = origin.z + radii.z;
            
            vertices[12] = origin.x + radii.x;
            vertices[13] = origin.y + radii.y;
            vertices[14] = origin.z;
            
            vertices[15] = origin.x + radii.x;
            vertices[16] = origin.y;
            vertices[17] = origin.z + radii.z;
            
            vertices[18] = origin.x;
            vertices[19] = origin.y + radii.y;
            vertices[20] = origin.z + radii.z;
            
            vertices[21] = origin.x + radii.x;
            vertices[22] = origin.y + radii.y;
            vertices[23] = origin.z + radii.z;
        }
        
        return vertices;
    }
private:
    
    GLfloat * vertices = nullptr;
};
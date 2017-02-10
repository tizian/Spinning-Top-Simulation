#include "OOBB.h"

#include<queue>

int maxNumberOfTriangles = 4;
int maxDepth = 30;
float minRadii = 0.000001;

OOBB::OOBB()
{
    setDefaults();
}

OOBB::OOBB(std::vector<Triangle> includedTriangles, glm::vec3 origin, glm::vec3 radii)
{
    setDefaults();
    
    m_includedTriangles = includedTriangles;
    
    m_origin = origin;
    m_radii = radii;
    
    setBoundingVertices();
}

OOBB::OOBB(Mesh * mesh) {
    setDefaults();
    
    std::vector<Triangle> includedTriangles = std::vector<Triangle>();
    
    bool equalVerticesInSameTriangle = false;
    
    GLfloat * vertices = mesh->getVertices();
    GLfloat * normals = mesh->getNormals();
    
    for (int i = 0; i < mesh->getNumVertices(); i += 9)
    {
        Triangle triangle;
        triangle.vertex1 = glm::vec3(vertices[i+0], vertices[i+1], vertices[i+2]);
        triangle.vertex2 = glm::vec3(vertices[i+3], vertices[i+4], vertices[i+5]);
        triangle.vertex3 = glm::vec3(vertices[i+6], vertices[i+7], vertices[i+8]);
        
        glm::vec3 normal1 = glm::vec3(normals[i+0], normals[i+1], normals[i+2]);
        glm::vec3 normal2 = glm::vec3(normals[i+3], normals[i+4], normals[i+5]);
        glm::vec3 normal3 = glm::vec3(normals[i+6], normals[i+7], normals[i+8]);
        
        triangle.normal = glm::normalize(normal1 + normal2 + normal3);
        
        includedTriangles.push_back(triangle);
        
        if (triangle.vertex1 == triangle.vertex2 || triangle.vertex1 == triangle.vertex3 || triangle.vertex2 == triangle.vertex3)
        {
            equalVerticesInSameTriangle = true;
        }
    }
    
    m_includedTriangles = includedTriangles;
    calculateBoundingBox();
    
    if (!equalVerticesInSameTriangle)
    {
        split(0);
        setDepths(0);
        print(false);
    } else {
        printf("ERROR: Could not create Octree, because there are equal vertices in the same triangles.\n");
    }
}

glm::vec3 OOBB::getOrigin() {
    return m_origin;
}


glm::vec3 OOBB::getRadii() {
    return m_radii;
}

GLuint OOBB::getNumVertices() {
    return 3*8; // To be consistent with Mesh.cpp
}

GLfloat * OOBB::getVertices() {
    return m_vertices;
}

int OOBB::getDepth()
{
    return m_depth;
}

std::vector<Triangle> OOBB::getIncludedTriangles() {
    return m_includedTriangles;
}

std::vector<OOBB> * OOBB::getChildren()
{
    return &m_children;
}

void OOBB::split(int depth) {
    if (m_children.size() == 0 && m_includedTriangles.size()/3 > maxNumberOfTriangles && depth < maxDepth && m_radii.x > 2.f * minRadii && m_radii.y > 2.f * minRadii && m_radii.z > 2.f * minRadii) {
        glm::vec3 childRadii = m_radii * 0.5f;
        
        glm::vec3 child1Origin = m_origin;
        glm::vec3 child2Origin = m_origin + glm::vec3(childRadii.x, 0, 0);
        glm::vec3 child3Origin = m_origin + glm::vec3(0, childRadii.y, 0);
        glm::vec3 child4Origin = m_origin + glm::vec3(0, 0, childRadii.z);
        glm::vec3 child5Origin = m_origin + glm::vec3(childRadii.x, childRadii.y, 0);
        glm::vec3 child6Origin = m_origin + glm::vec3(childRadii.x, 0, childRadii.z);
        glm::vec3 child7Origin = m_origin + glm::vec3(0, childRadii.y, childRadii.z);
        glm::vec3 child8Origin = m_origin + glm::vec3(childRadii.x, childRadii.y, childRadii.z);
        
        std::vector<glm::vec3> origins =std::vector<glm::vec3>();
        origins.push_back(child1Origin);
        origins.push_back(child2Origin);
        origins.push_back(child3Origin);
        origins.push_back(child4Origin);
        origins.push_back(child5Origin);
        origins.push_back(child6Origin);
        origins.push_back(child7Origin);
        origins.push_back(child8Origin);
        
        std::vector<Triangle> child1Triangles = std::vector<Triangle>();
        std::vector<Triangle> child2Triangles = std::vector<Triangle>();
        std::vector<Triangle> child3Triangles = std::vector<Triangle>();
        std::vector<Triangle> child4Triangles = std::vector<Triangle>();
        std::vector<Triangle> child5Triangles = std::vector<Triangle>();
        std::vector<Triangle> child6Triangles = std::vector<Triangle>();
        std::vector<Triangle> child7Triangles = std::vector<Triangle>();
        std::vector<Triangle> child8Triangles = std::vector<Triangle>();
        
        std::vector<std::vector<Triangle> > triangles =std::vector<std::vector<Triangle> >();
        triangles.push_back(child1Triangles);
        triangles.push_back(child2Triangles);
        triangles.push_back(child3Triangles);
        triangles.push_back(child4Triangles);
        triangles.push_back(child5Triangles);
        triangles.push_back(child6Triangles);
        triangles.push_back(child7Triangles);
        triangles.push_back(child8Triangles);
        
        for (int i = 0; i < m_includedTriangles.size(); ++i) {
            glm::vec3 point1 = m_includedTriangles[i].vertex1;
            glm::vec3 point2 = m_includedTriangles[i].vertex2;
            glm::vec3 point3 = m_includedTriangles[i].vertex3;
            
            for (int j = 0; j < origins.size(); ++j) {
                if (IntersectionTest::intersectionTriangleBox(point1, point2, point3, origins[j], childRadii)) {
                    triangles[j].push_back(m_includedTriangles[i]);
                }
            }
        }
        
        for (int i = 0; i < triangles.size(); ++i) {
            if (triangles[i].size() > 0)
            {
                OOBB child = OOBB(triangles[i], origins[i], childRadii);
                child.split(depth + 1);
                if (child.getChildren()->size() == 1)
                {
                    m_children.push_back(child.getChildren()->at(0));
                } else {
                    m_children.push_back(child);
                }
            }
        }
    }
}

void OOBB::realPrint(int depth)
{
    for (int i = 0; i < depth; ++i) {
        printf("\t");
    }
    
    printf("OOBB includedTriangles: %lu children: %lu depth: %d\n", m_includedTriangles.size()/3, m_children.size(), m_depth);
    // printf("OOBB includedTriangles: %lu origin: %f %f %f radii: %f %f %f children: %lu\n", m_includedTriangles.size()/3, m_origin.x, m_origin.y, m_origin.z, m_radii.x,m_radii.y, m_radii.z, children.size());
    
    for (int i = 0; i < m_children.size(); ++i) {
        m_children[i].realPrint(depth + 1);
    }
}

void OOBB::setDepths(int depth)
{
    m_depth = depth;
    for (int i = 0; i < m_children.size(); ++i) {
        m_children[i].setDepths(depth + 1);
    }
}


void OOBB::print(bool recursive)
{
    if (recursive) {
        realPrint(0);
    } else {
        int totalTrianglesOnLowestLevel = 0;
        int totalNodeCount = 0;
        int totalDepth = 0;
        std::queue<OOBB> toVisit = std::queue<OOBB>();
        toVisit.push(*this);
        
        while (!toVisit.empty()) {
            totalNodeCount++;
            
            OOBB next = toVisit.front();
            toVisit.pop();
            
            for (int i = 0; i < next.getChildren()->size(); ++i) {
                toVisit.push(next.getChildren()->at(i));
            }
            
            if (next.getChildren()->size() == 0)
            {
                totalTrianglesOnLowestLevel += next.getIncludedTriangles().size()/3;
                totalDepth = std::max(totalDepth, next.getDepth());
            }
        }
        
        printf("OOBB origin: %f %f %f radii: %f %f %f includedTriangles: %lu totalTrianglesOnLowestLevel: %d totalNodeCount: %d depth: %d\n", m_origin.x, m_origin.y, m_origin.z, m_radii.x,m_radii.y, m_radii.z, m_includedTriangles.size()/3, totalTrianglesOnLowestLevel, totalNodeCount, totalDepth);
    }   
}

void OOBB::setDefaults() {
    m_origin = glm::vec3(0,0,0);
    m_radii = glm::vec3(0,0,0);
    
    std::vector<GLfloat> m_includedTriangles = std::vector<GLfloat>();
    m_children = std::vector<OOBB>();
    
    m_depth = 0;
}

void OOBB::calculateBoundingBox() {
    std::vector<Triangle> includedTriangles = m_includedTriangles;
    
    // calculate origin and radii
    m_origin = glm::vec3(0,0,0);
    glm::vec3 m_max = glm::vec3(0,0,0);
    
    for (int i = 0; i < includedTriangles.size(); i++)
    {
        glm::vec3 vertex = includedTriangles[i].vertex1;
        
        m_origin = glm::vec3(std::min(m_origin.x, vertex.x),std::min(m_origin.y, vertex.y),std::min(m_origin.z, vertex.z));
        m_max = glm::vec3(std::max(m_max.x, vertex.x), std::max(m_max.y, vertex.y), std::max(m_max.z, vertex.z));
        
        vertex = includedTriangles[i].vertex2;
        
        m_origin = glm::vec3(std::min(m_origin.x, vertex.x),std::min(m_origin.y, vertex.y),std::min(m_origin.z, vertex.z));
        m_max = glm::vec3(std::max(m_max.x, vertex.x), std::max(m_max.y, vertex.y), std::max(m_max.z, vertex.z));
        
        vertex = includedTriangles[i].vertex3;
        
        m_origin = glm::vec3(std::min(m_origin.x, vertex.x),std::min(m_origin.y, vertex.y),std::min(m_origin.z, vertex.z));
        m_max = glm::vec3(std::max(m_max.x, vertex.x), std::max(m_max.y, vertex.y), std::max(m_max.z, vertex.z));
    }
    
    m_radii = m_max - m_origin;
    
    // add offset to ensure enclosure
    m_origin -= glm::vec3(0.1, 0.1, 0.1);
    m_radii += glm::vec3(0.2, 0.2, 0.2);
    
    setBoundingVertices();
}

void OOBB::setBoundingVertices()
{
    // calculate my own vertices
    m_vertices = new GLfloat[3*8];
    
    m_vertices[0] = m_origin.x;
    m_vertices[1] = m_origin.y;
    m_vertices[2] = m_origin.z;
    
    m_vertices[3] = m_origin.x + m_radii.x;
    m_vertices[4] = m_origin.y;
    m_vertices[5] = m_origin.z;
    
    m_vertices[6] = m_origin.x + m_radii.x;
    m_vertices[7] = m_origin.y + m_radii.y;
    m_vertices[8] = m_origin.z;
    
    m_vertices[9] = m_origin.x + m_radii.x;
    m_vertices[10] = m_origin.y + m_radii.y;
    m_vertices[11] = m_origin.z + m_radii.z;
    
    m_vertices[12] = m_origin.x + m_radii.x;
    m_vertices[13] = m_origin.y;
    m_vertices[14] = m_origin.z + m_radii.z;
    
    m_vertices[15] = m_origin.x;
    m_vertices[16] = m_origin.y;
    m_vertices[17] = m_origin.z + m_radii.z;
    
    m_vertices[18] = m_origin.x;
    m_vertices[19] = m_origin.y + m_radii.y;
    m_vertices[20] = m_origin.z + m_radii.z;
    
    m_vertices[21] = m_origin.x;
    m_vertices[22] = m_origin.y + m_radii.y;
    m_vertices[23] = m_origin.z;
}
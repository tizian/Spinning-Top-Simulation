#pragma once

#include "IntersectionTest.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace glm;

namespace InertiaTensor {
    
    const int NUMBER_SAMPLES = 1000000;
    
    // using the raytracing approach
    mat3 calculateInertiaTensor(RigidBody *body) {
        srand(0);
        
        int sampleCount = NUMBER_SAMPLES; // should be higher! But then we need to avoid calculating everything when 1, 2 or 3 is pressed.
        mat3 bodyInertiaTensor = mat3({0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f});
        
        int realSampleCount = 0;
        
        Mesh * mesh = body->getMesh();
        
        if (mesh != nullptr) {
            vec3 origin = body->getBoundingBox()->getOrigin();
            vec3 radii = body->getBoundingBox()->getRadii();
            
            int numVertices = mesh->getNumVertices();
            GLfloat * vertices = mesh->getVertices();
            
            // do monte-carlo integration by using sampleCount samples inside bounding box
            for (int i = 0; i < sampleCount; ++i) {
                if (i % 1000 == 0) {
                    printf("i: %d\n", i);
                }
                // get sample inside boundingbox
                vec3 sample = origin + vec3((float)rand() / (float)RAND_MAX * radii.x, (float)rand() / (float)RAND_MAX * radii.y, (float)rand() / (float)RAND_MAX * radii.z);
                // printf("sample: %f %f %f\n", sample.x, sample.y, sample.z);
                vec3 rayOrigin = origin;
                vec3 rayDirection = sample - origin;
                float theT = sqrt(rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z); // needed later
                rayDirection /= theT;
                std::vector<float> ts = std::vector<float>();
                
                // printf("theT: %f\n", theT);
                
                for (int j = 0; j < numVertices; j += 9) {
                    vec3 vertex1 = vec3(vertices[j], vertices[j+1], vertices[j+2]);
                    vec3 vertex2 = vec3(vertices[j+3], vertices[j+4], vertices[j+5]);
                    vec3 vertex3 = vec3(vertices[j+6], vertices[j+7], vertices[j+8]);
                    
                    // printf("v1: %f %f %f\nv2: %f %f %f\nv3: %f %f %f\n");
                    
                    float t = 0;
                    
                    if (IntersectionTest::intersectionRayTriangle(vertex1, vertex2, vertex3, rayOrigin, rayDirection, t)) {
                        ts.push_back(t);
                    }
                }
                
                sort(ts.begin(), ts.end());
                
                bool outside = true; // where is our sample point
                
                for (size_t j = 0; j < ts.size(); ++j) {
                    // printf("ts[%d]: %f\n", j, ts[j]);
                    if (ts[j] < theT) {
                        outside = !outside;
                    } else {
                        break;
                    }
                }
                
                if (!outside) {
                    // m_i is 1
                    // x(t) is (0,0,0) since we operate in objectspace.
                    
                    bodyInertiaTensor += mat3({std::pow(sample.y, 2.f)+std::pow(sample.z,2.f), -1 * sample.x * sample.y, -1 * sample.x * sample.z,
                                            -1 * sample.y * sample.x, std::pow(sample.x, 2.f) + std::pow(sample.z, 2.f), -1 * sample.y * sample.z,
                                            -1 * sample.z * sample.x, -1 * sample.z * sample.y, std::pow(sample.x, 2.f) + std::pow(sample.y, 2.f)});
                    realSampleCount++;
                }
            }
            
            if (realSampleCount > 0) {
                bodyInertiaTensor /= (float)realSampleCount;
            }
            
            printf("Hits inside object: %d\n", realSampleCount);
        }
        
        if (mesh == nullptr || realSampleCount == 0) {
            bodyInertiaTensor = glm::diagonal3x3(glm::vec3(2.0f/5.0f));
        }
        
        return glm::inverse(bodyInertiaTensor);
    }
};
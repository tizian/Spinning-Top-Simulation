#pragma once

namespace InertiaTensor {
    
    const int NUMBER_SAMPLES = 1000000;
    
    // http://en.wikipedia.org/wiki/Möller–Trumbore_intersection_algorithm
    int triangle_intersection( const vec3   V1,  // Triangle vertices
                              const vec3   V2,
                              const vec3   V3,
                              const vec3    O,  //Ray origin
                              const vec3    D,  //Ray direction
                              float* out )
    {
        vec3 e1, e2;  //Edge1, Edge2
        vec3 P, Q, T;
        float det, inv_det, u, v;
        float t;
        float eps = 0.000001;
        
        //Find vectors for two edges sharing V1
        e1 = V2 - V1;
        e2 = V3 - V1;
        //Begin calculating determinant - also used to calculate u parameter
        P = cross(D, e2);
        //if determinant is near zero, ray lies in plane of triangle
        det = dot(e1, P);
        //NOT CULLING
        if(det > -eps && det < eps) return 0;
        inv_det = 1.f / det;
        
        //calculate distance from V1 to ray origin
        T = O - V1;
        
        //Calculate u parameter and test bound
        u = dot(T, P) * inv_det;
        //The intersection lies outside of the triangle
        if(u < 0.f || u > 1.f) return 0;
        
        //Prepare to test v parameter
        Q = cross(T, e1);
        
        //Calculate V parameter and test bound
        v = dot(D, Q) * inv_det;
        //The intersection lies outside of the triangle
        if(v < 0.f || u + v  > 1.f) return 0;
        
        t = dot(e2, Q) * inv_det;
        
        if(t > eps) { //ray intersection
            *out = t;
            return 1;
        }
        
        // No hit, no win
        return 0;
    }
    
    
    // using the raytracing approach
    mat3 calculateInertiaTensor(Body * body)
    {
        srand(0);
        
        int sampleCount = NUMBER_SAMPLES; // should be higher! But then we need to avoid calculating everything when 1, 2 or 3 is pressed.
        mat3 bodyInertiaTensor = mat3({0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f});
        
        int realSampleCount = 0;
        
        Mesh * mesh = body->getMesh();
        
        if (mesh != nullptr)
        {
            vec3 origin = body->getBoundingBox().origin;
            vec3 radii = body->getBoundingBox().radii;
            
            int numVertices = mesh->getNumVertices();
            GLfloat * vertices = mesh->getVertices();
            
            // do monte-carlo integration by using sampleCount samples inside bounding box
            for (int i = 0; i < sampleCount; ++i)
            {
                if (i % 1000 == 0) {
                    printf("i: %d\n", i);
                }
                // get sample inside boundingbox
                vec3 sample = origin + vec3((float)rand() / (float)RAND_MAX * radii.x, (float)rand() / (float)RAND_MAX * radii.y, (float)rand() / (float)RAND_MAX * radii.z);
                //printf("sample: %f %f %f\n", sample.x, sample.y, sample.z);
                vec3 rayOrigin = origin;
                vec3 rayDirection = sample - origin;
                float theT = sqrt(rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z); // needed later
                rayDirection /= theT;
                std::vector<float> ts = std::vector<float>();
                
                //printf("theT: %f\n", theT);
                
                for (int j = 0; j < numVertices; j += 9)
                {
                    vec3 vertex1 = vec3(vertices[j], vertices[j+1], vertices[j+2]);
                    vec3 vertex2 = vec3(vertices[j+3], vertices[j+4], vertices[j+5]);
                    vec3 vertex3 = vec3(vertices[j+6], vertices[j+7], vertices[j+8]);
                    
                    //printf("v1: %f %f %f\nv2: %f %f %f\nv3: %f %f %f\n");
                    
                    float t = 0;
                    
                    if (triangle_intersection(vertex1, vertex2, vertex3, rayOrigin, rayDirection, &t) == 1)
                    {
                        ts.push_back(t);
                    }
                }
                
                sort(ts.begin(), ts.end());
                
                bool outside = true; // where is our sample point
                
                for (int j = 0; j < ts.size(); ++j)
                {
                    //printf("ts[%d]: %f\n", j, ts[j]);
                    if (ts[j] < theT)
                    {
                        outside = !outside;
                    } else {
                        break;
                    }
                }
                
                if (!outside)
                {
                    // m_i is 1
                    // x(t) is (0,0,0) since we operate in objectspace.
                    
                    bodyInertiaTensor += mat3({std::pow(sample.y, 2.f)+std::pow(sample.z,2.f), -1 * sample.x * sample.y, -1 * sample.x * sample.z,
                        -1 * sample.y * sample.x, std::pow(sample.x, 2.f) + std::pow(sample.z, 2.f), -1 * sample.y * sample.z,
                        -1 * sample.z * sample.x, -1 * sample.z * sample.y, std::pow(sample.x, 2.f) + std::pow(sample.y, 2.f)});
                    
                    realSampleCount++;
                }
                
            }
            
            if (realSampleCount > 0)
            {
                bodyInertiaTensor /= (float)realSampleCount;
            }
            
            printf("Hits inside object: %d\n", realSampleCount);
            
        }
        
        if (mesh == nullptr || realSampleCount == 0) {
            bodyInertiaTensor = glm::diagonal3x3(glm::vec3(2.0f/5.0f));
        }
        
        return glm::inverse(bodyInertiaTensor);
    }
}
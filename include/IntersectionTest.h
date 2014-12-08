
namespace IntersectionTest {

    // http://en.wikipedia.org/wiki/Möller–Trumbore_intersection_algorithm
    static bool intersectionRayTriangle(const glm::vec3 point1, const glm::vec3 point2, const glm::vec3 point3, const glm::vec3 rayOrigin, const glm::vec3 rayDirection, float * out)
    {
        glm::vec3 e1, e2;  //Edge1, Edge2
        glm::vec3 P, Q, T;
        float det, inv_det, u, v;
        float t;
        float eps = 0.000001;
        
        //Find vectors for two edges sharing point1
        e1 = point2 - point1;
        e2 = point3 - point1;
        //Begin calculating determinant - also used to calculate u parameter
        P = glm::cross(rayDirection, e2);
        //if determinant is near zero, ray lies in plane of triangle
        det = glm::dot(e1, P);
        //NOT CULLING
        if(det > -eps && det < eps) return false;
        inv_det = 1.f / det;
        
        //calculate distance from point1 to ray origin
        T = rayOrigin - point1;
        
        //Calculate u parameter and test bound
        u = glm::dot(T, P) * inv_det;
        //The intersection lies outside of the triangle
        if(u < 0.f || u > 1.f) return false;
        
        //Prepare to test v parameter
        Q = glm::cross(T, e1);
        
        //Calculate V parameter and test bound
        v = glm::dot(rayDirection, Q) * inv_det;
        //The intersection lies outside of the triangle
        if(v < 0.f || u + v  > 1.f) return false;
        
        t = glm::dot(e2, Q) * inv_det;
        
        if(t > eps) { //ray intersection
            *out = t;
            return true;
        }
        
        // No hit, no win
        return false;
    }

    // https://graphics.ethz.ch/teaching/cg14/downloads/slides/04_Raytracing_3.pdf
    static bool intersectionRayBox(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 boxOrigin, glm::vec3 boxRadii, float * t)
    {
        float tmin1 = 0.f;
        float tmax1 = 0.f;
        
        if (rayDirection.x != 0.f && rayDirection.y != 0.f)
        {
            tmin1 = std::max((boxOrigin.x - rayOrigin.x) / rayDirection.x, (boxOrigin.y - rayOrigin.y) / rayDirection.y);
            tmax1 = std::min((boxOrigin.x + boxRadii.x - rayOrigin.x) / rayDirection.x, (boxOrigin.y + boxRadii.y - rayOrigin.y) / rayDirection.y);
        }
        
        float tmin2 = 0.f;
        float tmax2 = 0.f;
        
        if (rayDirection.z != 0.f && rayDirection.y != 0.f)
        {
            tmin1 = std::max((boxOrigin.z - rayOrigin.z) / rayDirection.z, (boxOrigin.y - rayOrigin.y) / rayDirection.y);
            tmax1 = std::min((boxOrigin.z + boxRadii.z - rayOrigin.z) / rayDirection.z, (boxOrigin.y + boxRadii.y - rayOrigin.y) / rayDirection.y);
        }
        
        float tmin3 = 0.f;
        float tmax3 = 0.f;
        
        if (rayDirection.x != 0.f && rayDirection.z != 0.f)
        {
            tmin1 = std::max((boxOrigin.z - rayOrigin.z) / rayDirection.z, (boxOrigin.x - rayOrigin.x) / rayDirection.x);
            tmax1 = std::min((boxOrigin.z + boxRadii.z - rayOrigin.z) / rayDirection.z, (boxOrigin.x + boxRadii.x - rayOrigin.x) / rayDirection.x);
        }
        
        if (rayDirection.x != 0 && rayDirection.y != 0 && rayDirection.z != 0)
        {
            *t = tmax1;
            return (tmin1 < tmax1 && tmin2 < tmax2 && tmin3 < tmax3);
        } else if (rayDirection.x != 0 && rayDirection.y != 0)
        {
            *t = tmax1;
            return (tmin1 < tmax1 && rayOrigin.z >= boxOrigin.z && rayOrigin.z <= boxOrigin.z + boxRadii.z);
        } else if (rayDirection.z != 0 && rayDirection.y != 0) {
            *t = tmax2;
            return (tmin2 < tmax2 && rayOrigin.x >= boxOrigin.x && rayOrigin.x <= boxOrigin.x + boxRadii.x);
        } else if (rayDirection.z != 0 && rayDirection.x != 0) {
            *t = tmax3;
            return (tmin3 < tmax3 && rayOrigin.y >= boxOrigin.y && rayOrigin.y <= boxOrigin.y + boxRadii.y);
        } else if (rayDirection.x != 0) {
            *t = boxOrigin.x - rayOrigin.x / rayDirection.x;
            return (rayOrigin.y >= boxOrigin.y && rayOrigin.y <= boxOrigin.y + boxRadii.y && rayOrigin.z >= boxOrigin.z && rayOrigin.z <= boxOrigin.z + boxRadii.z);
        } else if (rayDirection.y != 0) {
            *t = boxOrigin.y - rayOrigin.y / rayDirection.y;
            return (rayOrigin.x >= boxOrigin.x && rayOrigin.x <= boxOrigin.x + boxRadii.x && rayOrigin.z >= boxOrigin.z && rayOrigin.z <= boxOrigin.z + boxRadii.z);
        } else if (rayDirection.z != 0) {
            *t = boxOrigin.z - rayOrigin.z / rayDirection.z;
            return (rayOrigin.y >= boxOrigin.y && rayOrigin.y <= boxOrigin.y + boxRadii.y && rayOrigin.x >= boxOrigin.x && rayOrigin.x <= boxOrigin.x + boxRadii.x);
        }
        
        printf("ERROR: Ray - Box intersection got something wrong.\n\trayDirection: %f %f %f\n", rayDirection.x, rayDirection.y, rayDirection.z); // This line should never be reached.
        
        return false;
    }
    
    static bool intersectionTriangleBox(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 origin, glm::vec3 radii)
    {
        if (point1 == point2 || point1 == point3 || point2 == point3)
        {
            printf("ERROR: Triangle - Box intersection. Input is not a triangle (Two equal points)\n");
        }
        // sort out the easy cases
        
        // below the AABB
        if (point1.x < origin.x && point2.x < origin.x && point3.x < origin.x)
        {
            return false;
        }
        
        if (point1.y < origin.y && point2.y < origin.y && point3.y < origin.y)
        {
            return false;
        }
        
        if (point1.z < origin.z && point2.z < origin.z && point3.z < origin.z)
        {
            return false;
        }
        
        // above the AABB
        if (point1.x > origin.x + radii.x && point2.x > origin.x + radii.x && point3.x > origin.x + radii.x)
        {
            return false;
        }
        
        if (point1.y > origin.y + radii.y && point2.y > origin.y + radii.y && point3.y > origin.y + radii.y)
        {
            return false;
        }
        
        if (point1.z > origin.z + radii.z && point2.z > origin.z + radii.z && point3.z > origin.z + radii.z)
        {
            return false;
        }
        
        // check if a vertex is inside the box
        if (point1.x >= origin.x && point1.x <= origin.x + radii.x &&
            point1.y >= origin.y && point1.y <= origin.y + radii.y &&
            point1.z >= origin.z && point1.z <= origin.z + radii.z) {
            return true;
        } else if (point2.x >= origin.x && point2.x <= origin.x + radii.x &&
                   point2.y >= origin.y && point2.y <= origin.y + radii.y &&
                   point2.z >= origin.z && point2.z <= origin.z + radii.z) {
            return true;
        } else if (point3.x >= origin.x && point3.x <= origin.x + radii.x &&
                   point3.y >= origin.y && point3.y <= origin.y + radii.y &&
                   point3.z >= origin.z && point3.z <= origin.z + radii.z) {
            return true;
        }
        
        // check if a line intersects the box
        
        float t;
        
        glm::vec3 origin1 = point1;
        glm::vec3 direction1 = point2 - point1;
        
        if (intersectionRayBox(origin1, direction1, origin, radii, &t))
        {
            if (t >= 0 && t <= 1)
            {
                return true;
            }
        }
        
        glm::vec3 origin2 = point2;
        glm::vec3 direction2 = point3 - point2;
        
        if (intersectionRayBox(origin2, direction2, origin, radii, &t))
        {
            if (t >= 0 && t <= 1)
            {
                return true;
            }
        }
        
        glm::vec3 origin3 = point3;
        glm::vec3 direction3 = point1 - point3;
        
        if (intersectionRayBox(origin3, direction3, origin, radii, &t))
        {
            if (t >= 0 && t <= 1)
            {
                return true;
            }
        }
        
        // check if the face intersects the box
        
        origin1 = origin;
        direction1 = radii;
        
        if (intersectionRayTriangle(point1, point2, point3, origin1, direction1, &t))
        {
            if (t >= 0 && t <= 1)
            {
                return true;
            }
        }
        
        origin2 = origin + glm::vec3(radii.x, 0, 0);
        direction2 = glm::vec3(-radii.x, radii.y, radii.z);
        
        if (intersectionRayTriangle(point1, point2, point3, origin2, direction2, &t))
        {
            if (t >= 0 && t <= 1)
            {
                return true;
            }
        }
        
        origin3 = origin + glm::vec3(0, radii.y, 0);
        direction3 = glm::vec3(radii.x, -radii.y, radii.z);
        
        if (intersectionRayTriangle(point1, point2, point3, origin3, direction3, &t))
        {
            if (t >= 0 && t <= 1)
            {
                return true;
            }
        }
        
        glm::vec3 origin4 = origin + glm::vec3(0, 0, radii.z);
        glm::vec3 direction4 = glm::vec3(radii.x, radii.y, -radii.z);
        
        if (intersectionRayTriangle(point1, point2, point3, origin4, direction4, &t))
        {
            if (t >= 0 && t <= 1)
            {
                return true;
            }
        }
        
        // no intersection
        
        return false;
    }
};
#pragma once

// Baraff and Witkin approach
class RigidBody;

struct Contact {
   	// RigidBody * a;  // for vertex-face case: RB with the vertex
   	// RigidBody * b;  // for vertex-face case: RB with the face
    
    glm::vec3 p;    // point

	glm::vec3 n;    // normal pointing away from B. n = cross(ea, eb) for the edge-edge case
    
	// glm::vec3 ea;   // edge of A for edge-edge case  (directions s.t. n is pointing away from B)
	// glm::vec3 eb;   // edge of B for edge-edge case
    
	// bool vf;        // vertex-face case or not
};
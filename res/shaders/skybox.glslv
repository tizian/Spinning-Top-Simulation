#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

// In: per vertex information in model space
layout(location = 0) in vec3 vPosition;      // In: (x,y,z) position
//layout(location = 1) in vec3 vNormal;        // In: (x,y,z) normal values

// Out: per fragment information
//out vec4 fPosition;		// Out: (x,y,z) position
//out vec3 fNormal;		// Out: (x,y,z) normal values
out vec3 texCoord;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
    texCoord = vPosition;
}
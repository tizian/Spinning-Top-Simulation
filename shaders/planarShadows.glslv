#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 planeMatrix;

// In: per vertex information in model space
layout(location = 0) in vec3 vPosition;      // In: (x,y,z) position

// Out: per fragment information
out vec4 fPosition;		// Out: (x,y,z) position

void main() {
    fPosition= planeMatrix * modelMatrix * vec4(vPosition, 1.);
    fPosition.y += 0.005;
    gl_Position = projectionMatrix * viewMatrix * fPosition;
}
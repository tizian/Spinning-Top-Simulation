#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

// In: per vertex information in model space
layout(location = 0) in vec3 vPosition;      // In: (x,y,z) position
layout(location = 1) in vec3 vNormal;        // In: (x,y,z) normal values
layout(location = 2) in vec2 vTexCoord;      // In: (u,v) texture coordinates

// Out: per fragment information
out vec4 fPosition;		// Out: (x,y,z) position
out vec3 fNormal;		// Out: (x,y,z) normal values
out vec2 fTexCoord;     // Out: (u,v) texture coordinates

void main() {
    fPosition= modelMatrix * vec4(vPosition, 1.);
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    
    fNormal = normalMatrix * vNormal;
    fTexCoord = vTexCoord;
    
    gl_Position = projectionMatrix * viewMatrix * fPosition;
}
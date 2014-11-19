#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

uniform vec3 cameraPosition;

uniform struct Light {
    vec3 position;
    vec3 color;
} light;

uniform struct Material {
    vec3 Ks;
    vec3 Kd;
    vec3 Ka;
    float shininess;
} material;

// In: per vertex information in model space
layout(location = 0) in vec3 vPosition;      // In: (x,y,z) position

// Out: per fragment information
out vec4 fPosition;		// Out: (x,y,z) position

void main() {
    mat4 planeMatrix = mat4(vec4(light.position.y, 0, 0, 0),
                            vec4(-light.position.x, 0, -light.position.z, -1),
                            vec4(0, 0, light.position.y, 0),
                            vec4(0, 0, 0, light.position.y));
    
    fPosition= planeMatrix * modelMatrix * vec4(vPosition, 1.);
    fPosition.y += 0.1;//0.005; // not so nice hack to get a clean shadow
    gl_Position = projectionMatrix * viewMatrix * fPosition;
    
    vec3 cameraPositionTmp = cameraPosition;
    vec3 materialKsTmp = material.Ks;
}
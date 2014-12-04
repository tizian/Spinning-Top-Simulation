#version 330

in vec3 texCoord;
out vec4 fragColor;

uniform vec3 cameraPosition;

uniform samplerCube tex;

void main() {
    fragColor = texture(tex, texCoord);
    
    vec3 cPos = cameraPosition;
}
#version 150

uniform vec3 cameraPosition;

uniform mat4 viewMatrix;

uniform struct Material {
    vec3 Ks;
    vec3 Kd;
    vec3 Ka;
    float shininess;
} material;

uniform struct Light {
    vec3 position;
    vec3 color;
} light;

uniform sampler2D tex;

// In: per fragment information
in vec4 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;

// Out: final color of this fragmant
out vec4 fragColor;

void main() {
    vec3 P = fPosition.xyz;
    vec3 V = normalize(cameraPosition - P);
    vec3 N = normalize(fNormal);
    
    vec3 ambient = 1 * light.color * material.Ka;
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);
    
    vec3 L = normalize(light.position - P);
    float LN = clamp(dot(L, N), 0.0, 1.0);
    
    diffuse += light.color * material.Kd * LN;
    
    vec3 H = normalize(L + V);
    float NH = dot(N, H);
    
    float kSpec = 0.0;
    if (dot(N, L) > 0.0) {
        kSpec = pow(NH, material.shininess);
    }
    
    specular += light.color * material.Ks * kSpec;
    
    vec3 color = ambient + diffuse + specular;
    vec4 surfaceColor = texture(tex, fTexCoord);
    
    fragColor = clamp(surfaceColor * vec4(color, 1.), 0., 1.);
}